}
static vlc_fourcc_t ParseFormat (vlc_object_t *obj,
                                 const xcb_xv_image_format_info_t *restrict f)
{
    switch (f->type)
    {
      case XCB_XV_IMAGE_FORMAT_INFO_TYPE_RGB:
        switch (f->num_planes)
        {
          case 1:
            switch (popcount (f->red_mask | f->green_mask | f->blue_mask))
            {
              case 24:
                if (f->bpp == 32 && f->depth == 32)
                    return VLC_CODEC_ARGB;
                if (f->bpp == 32 && f->depth == 24)
                    return VLC_CODEC_RGB32;
                if (f->bpp == 24 && f->depth == 24)
                    return VLC_CODEC_RGB24;
                break;
              case 16:
                if (f->byte_order != ORDER)
                    return 0; /* Mixed endian! */
                if (f->bpp == 16 && f->depth == 16)
                    return VLC_CODEC_RGB16;
                break;
              case 15:
                if (f->byte_order != ORDER)
                    return 0; /* Mixed endian! */
                if (f->bpp == 16 && f->depth == 15)
                    return VLC_CODEC_RGB15;
                break;
              case 12:
                if (f->bpp == 16 && f->depth == 12)
                    return VLC_CODEC_RGB12;
                break;
              case 8:
                if (f->bpp == 8 && f->depth == 8)
                    return VLC_CODEC_RGB8;
                break;
            }
            break;
        }
        msg_Err (obj, "unknown XVideo RGB format %"PRIx32" (%.4s)",
                 f->id, f->guid);
        msg_Dbg (obj, " %"PRIu8" planes, %"PRIu8" bits/pixel, "
                 "depth %"PRIu8, f->num_planes, f->bpp, f->depth);
        break;
      case XCB_XV_IMAGE_FORMAT_INFO_TYPE_YUV:
        if (f->u_sample_bits != f->v_sample_bits
         || f->vhorz_u_period != f->vhorz_v_period
         || f->vvert_u_period != f->vvert_v_period
         || f->y_sample_bits != 8 || f->u_sample_bits != 8
         || f->vhorz_y_period != 1 || f->vvert_y_period != 1)
            goto bad;
        switch (f->num_planes)
        {
          case 1:
            switch (f->bpp)
            {
              /*untested: case 24:
                if (f->vhorz_u_period == 1 && f->vvert_u_period == 1)
                    return VLC_CODEC_I444;
                break;*/
              case 16:
                if (f->vhorz_u_period == 2 && f->vvert_u_period == 1)
                {
                    if (!strcmp ((const char *)f->vcomp_order, "YUYV"))
                        return VLC_CODEC_YUYV;
                    if (!strcmp ((const char *)f->vcomp_order, "UYVY"))
                        return VLC_CODEC_UYVY;
                }
                break;
            }
            break;
          case 3:
            switch (f->bpp)
            {
              case 12:
                if (f->vhorz_u_period == 2 && f->vvert_u_period == 2)
                {
                    if (!strcmp ((const char *)f->vcomp_order, "YVU"))
                        return VLC_CODEC_YV12;
                    if (!strcmp ((const char *)f->vcomp_order, "YUV"))
                        return VLC_CODEC_I420;
                }
            }
            break;
        }
    bad:
        msg_Err (obj, "unknown XVideo YUV format %"PRIx32" (%.4s)", f->id,
                 f->guid);
        msg_Dbg (obj, " %"PRIu8" planes, %"PRIu32" bits/pixel, "
                 "%"PRIu32"/%"PRIu32"/%"PRIu32" bits/sample", f->num_planes,
                 f->bpp, f->y_sample_bits, f->u_sample_bits, f->v_sample_bits);
        msg_Dbg (obj, " period: %"PRIu32"/%"PRIu32"/%"PRIu32"x"
                 "%"PRIu32"/%"PRIu32"/%"PRIu32,
                 f->vhorz_y_period, f->vhorz_u_period, f->vhorz_v_period,
                 f->vvert_y_period, f->vvert_u_period, f->vvert_v_period);
        msg_Warn (obj, " order: %.32s", f->vcomp_order);
        break;
    }
    return 0;
}
