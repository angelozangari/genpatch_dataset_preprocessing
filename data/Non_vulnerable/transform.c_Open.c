}
static int Open(vlc_object_t *object)
{
    filter_t *filter = (filter_t *)object;
    const video_format_t *src = &filter->fmt_in.video;
    video_format_t       *dst = &filter->fmt_out.video;
    const vlc_chroma_description_t *chroma =
        vlc_fourcc_GetChromaDescription(src->i_chroma);
    if (chroma == NULL)
        return VLC_EGENERIC;
    filter_sys_t *sys = malloc(sizeof(*sys));
    if (!sys)
        return VLC_ENOMEM;
    sys->chroma = chroma;
    static const char *const ppsz_filter_options[] = {
        "type", NULL
    };
    config_ChainParse(filter, CFG_PREFIX, ppsz_filter_options,
                      filter->p_cfg);
    char *type_name = var_InheritString(filter, CFG_PREFIX"type");
    const transform_description_t *dsc = NULL;
    for (size_t i = 0; i < n_transforms; i++)
        if (type_name && !strcmp(descriptions[i].name, type_name)) {
            dsc = &descriptions[i];
            break;
        }
    if (dsc == NULL) {
        dsc = &descriptions[0];
        msg_Warn(filter, "No valid transform mode provided, using '%s'",
                 dsc->name);
    }
    free(type_name);
    switch (chroma->pixel_size) {
        case 1:
            sys->plane[0] = dsc->plane8;
            break;
        case 2:
            sys->plane[0] = dsc->plane16;
            break;
        case 4:
            sys->plane[0] = dsc->plane32;
            break;
        default:
            msg_Err(filter, "Unsupported pixel size %u (chroma %4.4s)",
                    chroma->pixel_size, (char *)&src->i_chroma);
            goto error;
    }
    for (unsigned i = 1; i < PICTURE_PLANE_MAX; i++)
        sys->plane[i] = sys->plane[0];
    sys->convert = dsc->convert;
    if (dsc_is_rotated(dsc)) {
        switch (src->i_chroma) {
            case VLC_CODEC_I422:
            case VLC_CODEC_J422:
                sys->plane[2] = sys->plane[1] = dsc->i422;
                break;
            default:
                for (unsigned i = 0; i < chroma->plane_count; i++) {
                    if (chroma->p[i].w.num * chroma->p[i].h.den
                     != chroma->p[i].h.num * chroma->p[i].w.den) {
                        msg_Err(filter, "Format rotation not possible "
                                "(chroma %4.4s)", (char *)&src->i_chroma);
                        goto error;
                    }
            }
        }
    }
    /*
     * Note: we neither compare nor set dst->orientation,
     * the caller needs to do it manually (user might want
     * to transform video without changing the orientation).
     */
    video_format_t src_trans = *src;
    video_format_TransformBy(&src_trans, dsc->operation);
    if (!filter->b_allow_fmt_out_change &&
        (dst->i_width          != src_trans.i_width ||
         dst->i_visible_width  != src_trans.i_visible_width ||
         dst->i_height         != src_trans.i_height ||
         dst->i_visible_height != src_trans.i_visible_height ||
         dst->i_sar_num        != src_trans.i_sar_num ||
         dst->i_sar_den        != src_trans.i_sar_den ||
         dst->i_x_offset       != src_trans.i_x_offset ||
         dst->i_y_offset       != src_trans.i_y_offset)) {
            msg_Err(filter, "Format change is not allowed");
            goto error;
    }
    else if(filter->b_allow_fmt_out_change) {
        dst->i_width          = src_trans.i_width;
        dst->i_visible_width  = src_trans.i_visible_width;
        dst->i_height         = src_trans.i_height;
        dst->i_visible_height = src_trans.i_visible_height;
        dst->i_sar_num        = src_trans.i_sar_num;
        dst->i_sar_den        = src_trans.i_sar_den;
        dst->i_x_offset       = src_trans.i_x_offset;
        dst->i_y_offset       = src_trans.i_y_offset;
    }
    /* Deal with weird packed formats */
    switch (src->i_chroma) {
        case VLC_CODEC_UYVY:
        case VLC_CODEC_VYUY:
            if (dsc_is_rotated(dsc)) {
                msg_Err(filter, "Format rotation not possible (chroma %4.4s)",
                        (char *)&src->i_chroma);
                goto error;
            }
            /* fallthrough */
        case VLC_CODEC_YUYV:
        case VLC_CODEC_YVYU:
            sys->plane[0] = dsc->yuyv; /* 32-bits, not 16-bits! */
            break;
        case VLC_CODEC_NV12:
        case VLC_CODEC_NV21:
            goto error;
    }
    filter->p_sys           = sys;
    filter->pf_video_filter = Filter;
    filter->pf_video_mouse  = Mouse;
    return VLC_SUCCESS;
error:
    free(sys);
    return VLC_EGENERIC;
}
