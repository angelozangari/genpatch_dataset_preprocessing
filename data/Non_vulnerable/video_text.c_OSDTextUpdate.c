}
static void OSDTextUpdate(subpicture_t *subpic,
                          const video_format_t *fmt_src,
                          const video_format_t *fmt_dst,
                          mtime_t ts)
{
    subpicture_updater_sys_t *sys = subpic->updater.p_sys;
    VLC_UNUSED(fmt_src); VLC_UNUSED(ts);
    if( fmt_dst->i_sar_num <= 0 || fmt_dst->i_sar_den <= 0 )
        return;
    subpic->b_absolute = false;
    subpic->i_original_picture_width  = fmt_dst->i_visible_width * fmt_dst->i_sar_num / fmt_dst->i_sar_den;
    subpic->i_original_picture_height = fmt_dst->i_visible_height;
    video_format_t fmt;
    video_format_Init( &fmt, VLC_CODEC_TEXT);
    fmt.i_sar_num = 1;
    fmt.i_sar_den = 1;
    subpicture_region_t *r = subpic->p_region = subpicture_region_New(&fmt);
    if (!r)
        return;
    r->psz_text = strdup(sys->text);
    const float margin_ratio = 0.04;
    const int   margin_h     = margin_ratio * fmt_dst->i_visible_width;
    const int   margin_v     = margin_ratio * fmt_dst->i_visible_height;
    r->i_align = sys->position;
    r->i_x = 0;
    if (r->i_align & SUBPICTURE_ALIGN_LEFT)
        r->i_x += margin_h + fmt_dst->i_x_offset;
    else if (r->i_align & SUBPICTURE_ALIGN_RIGHT)
        r->i_x += margin_h - fmt_dst->i_x_offset;
    r->i_y = 0;
    if (r->i_align & SUBPICTURE_ALIGN_TOP )
        r->i_y += margin_v + fmt_dst->i_y_offset;
    else if (r->i_align & SUBPICTURE_ALIGN_BOTTOM )
        r->i_y += margin_v - fmt_dst->i_y_offset;
}
