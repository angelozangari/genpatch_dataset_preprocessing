static xcb_xv_query_image_attributes_reply_t *
FindFormat (vlc_object_t *obj, xcb_connection_t *conn, video_format_t *fmt,
            const xcb_xv_adaptor_info_t *a, uint32_t *idp)
{
    /* Order chromas by preference */
    vlc_fourcc_t tab[7];
    const vlc_fourcc_t *chromav = tab;
    vlc_fourcc_t chroma = var_InheritInteger (obj, "xvideo-format-id");
    if (chroma != 0) /* Forced chroma */
    {
        tab[0] = chroma;
        tab[1] = 0;
    }
    else if (vlc_fourcc_IsYUV (fmt->i_chroma)) /* YUV chroma */
    {
        chromav = vlc_fourcc_GetYUVFallback (fmt->i_chroma);
    }
    else /* RGB chroma */
    {
        tab[0] = fmt->i_chroma;
        tab[1] = VLC_CODEC_RGB32;
        tab[2] = VLC_CODEC_RGB24;
        tab[3] = VLC_CODEC_RGB16;
        tab[4] = VLC_CODEC_RGB15;
        tab[5] = VLC_CODEC_YUYV;
        tab[6] = 0;
    }
    /* Get available image formats */
    xcb_xv_list_image_formats_reply_t *list =
        xcb_xv_list_image_formats_reply (conn,
            xcb_xv_list_image_formats (conn, a->base_id), NULL);
    if (list == NULL)
        return NULL;
    /* Check available XVideo chromas */
    xcb_xv_query_image_attributes_reply_t *attr = NULL;
    unsigned rank = UINT_MAX;
    for (const xcb_xv_image_format_info_t *f =
             xcb_xv_list_image_formats_format (list),
                                          *f_end =
             f + xcb_xv_list_image_formats_format_length (list);
         f < f_end;
         f++)
    {
        chroma = ParseFormat (obj, f);
        if (chroma == 0)
            continue;
        /* Oink oink! */
        if ((chroma == VLC_CODEC_I420 || chroma == VLC_CODEC_YV12)
         && a->name_size >= 4
         && !memcmp ("OMAP", xcb_xv_adaptor_info_name (a), 4))
        {
            msg_Dbg (obj, "skipping slow I420 format");
            continue; /* OMAP framebuffer sucks at YUV 4:2:0 */
        }
        if (!BetterFormat (chroma, chromav, &rank))
            continue;
        xcb_xv_query_image_attributes_reply_t *i;
        i = xcb_xv_query_image_attributes_reply (conn,
            xcb_xv_query_image_attributes (conn, a->base_id, f->id,
                                           fmt->i_visible_width,
                                           fmt->i_visible_height), NULL);
        if (i == NULL)
            continue;
        fmt->i_chroma = chroma;
        fmt->i_x_offset = 0;
        fmt->i_y_offset = 0;
        fmt->i_width = i->width;
        fmt->i_height = i->height;
        if (f->type == XCB_XV_IMAGE_FORMAT_INFO_TYPE_RGB)
        {
            fmt->i_rmask = f->red_mask;
            fmt->i_gmask = f->green_mask;
            fmt->i_bmask = f->blue_mask;
        }
        *idp = f->id;
        free (attr);
        attr = i;
        if (rank == 0)
            break; /* shortcut for perfect match */
    }
    free (list);
    return attr;
}
