}
static void Display(vout_display_t *vd, picture_t *picture, subpicture_t *subpicture)
{
    vout_display_sys_t *sys = vd->sys;
    /* */
    video_format_t fmt = vd->fmt;
    if (ORIENT_IS_SWAP(vd->source.orientation))
    {
        fmt.i_sar_num = vd->source.i_sar_den;
        fmt.i_sar_den = vd->source.i_sar_num;
    }
    else
    {
        fmt.i_sar_num = vd->source.i_sar_num;
        fmt.i_sar_den = vd->source.i_sar_den;
    }
    /* */
    char type;
    if (picture->b_progressive)
        type = 'p';
    else if (picture->b_top_field_first)
        type = 't';
    else
        type = 'b';
    if (type != 'p') {
        msg_Warn(vd, "Received a non progressive frame, "
                     "it will be written as progressive.");
        type = 'p';
    }
    /* */
    if (!sys->is_first) {
        const char *header;
        char buffer[5];
        if (sys->is_yuv4mpeg2) {
            /* MPlayer compatible header, unfortunately it doesn't tell you
             * the exact fourcc used. */
            header = "YUV4MPEG2";
        } else {
            snprintf(buffer, sizeof(buffer), "%4.4s", 
                     (const char*)&fmt.i_chroma);
            header = buffer;
        }
        fprintf(sys->f, "%s W%d H%d F%d:%d I%c A%d:%d\n",
                header,
                fmt.i_visible_width, fmt.i_visible_height,
                fmt.i_frame_rate, fmt.i_frame_rate_base,
                type,
                fmt.i_sar_num, fmt.i_sar_den);
        sys->is_first = true;
    }
    /* */
    fprintf(sys->f, "FRAME\n");
    for (int i = 0; i < picture->i_planes; i++) {
        const plane_t *plane = &picture->p[i];
        const uint8_t *pixels = plane->p_pixels;
        pixels += (vd->fmt.i_x_offset * plane->i_visible_pitch)
                  / vd->fmt.i_visible_height;
        for( int y = 0; y < plane->i_visible_lines; y++) {
            const size_t written = fwrite(pixels, 1, plane->i_visible_pitch,
                                          sys->f);
            if (written != (size_t)plane->i_visible_pitch)
                msg_Warn(vd, "only %zd of %d bytes written",
                         written, plane->i_visible_pitch);
            pixels += plane->i_pitch;
        }
    }
    fflush(sys->f);
    /* */
    picture_Release(picture);
    VLC_UNUSED(subpicture);
}
