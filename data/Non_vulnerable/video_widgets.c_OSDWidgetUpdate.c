}
static void OSDWidgetUpdate(subpicture_t *subpic,
                          const video_format_t *fmt_src,
                          const video_format_t *fmt_dst,
                          mtime_t ts)
{
    subpicture_updater_sys_t *sys = subpic->updater.p_sys;
    VLC_UNUSED(fmt_src); VLC_UNUSED(ts);
    video_format_t fmt = *fmt_dst;
    fmt.i_width         = fmt.i_width         * fmt.i_sar_num / fmt.i_sar_den;
    fmt.i_visible_width = fmt.i_visible_width * fmt.i_sar_num / fmt.i_sar_den;
    fmt.i_x_offset      = fmt.i_x_offset      * fmt.i_sar_num / fmt.i_sar_den;
    fmt.i_sar_num       = 1;
    fmt.i_sar_den       = 1;
    subpic->i_original_picture_width  = fmt.i_visible_width;
    subpic->i_original_picture_height = fmt.i_visible_height;
    if (sys->type == OSD_HOR_SLIDER || sys->type == OSD_VERT_SLIDER)
        subpic->p_region = OSDSlider(sys->type, sys->position, &fmt);
    else
        subpic->p_region = OSDIcon(sys->type, &fmt);
}
