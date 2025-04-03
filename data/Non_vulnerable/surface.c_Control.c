}
static int Control(vout_display_t *vd, int query, va_list args)
{
    VLC_UNUSED(args);
    switch (query) {
    case VOUT_DISPLAY_HIDE_MOUSE:
        return VLC_SUCCESS;
    case VOUT_DISPLAY_CHANGE_SOURCE_CROP:
    {
        if (!vd->sys)
            return VLC_EGENERIC;
        vout_display_sys_t *sys = vd->sys;
        const video_format_t *source = (const video_format_t *)va_arg(args, const video_format_t *);
        sys->fmt = *source;
        sys->b_changed_crop = true;
        return VLC_SUCCESS;
    }
    default:
        msg_Err(vd, "Unknown request in android vout display");
    case VOUT_DISPLAY_CHANGE_FULLSCREEN:
    case VOUT_DISPLAY_CHANGE_WINDOW_STATE:
    case VOUT_DISPLAY_CHANGE_DISPLAY_SIZE:
    case VOUT_DISPLAY_CHANGE_DISPLAY_FILLED:
    case VOUT_DISPLAY_CHANGE_ZOOM:
    case VOUT_DISPLAY_CHANGE_SOURCE_ASPECT:
    case VOUT_DISPLAY_GET_OPENGL:
        return VLC_EGENERIC;
    }
}
