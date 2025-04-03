}
static int Mouse(filter_t *filter, vlc_mouse_t *mouse,
                 const vlc_mouse_t *mold, const vlc_mouse_t *mnew)
{
    VLC_UNUSED( mold );
    const video_format_t *fmt = &filter->fmt_out.video;
    const filter_sys_t   *sys = filter->p_sys;
    *mouse = *mnew;
    sys->convert(&mouse->i_x, &mouse->i_y,
                 fmt->i_visible_width, fmt->i_visible_height,
                 mouse->i_x, mouse->i_y);
    return VLC_SUCCESS;
}
