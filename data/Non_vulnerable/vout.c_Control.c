}
static int Control(vout_display_t *vd, int query, va_list args)
{
    VLC_UNUSED(args);
    switch (query) {
    case VOUT_DISPLAY_HIDE_MOUSE:
        return VLC_SUCCESS;
    default:
        msg_Err(vd, "Unknown request in omxil vout display");
    case VOUT_DISPLAY_CHANGE_SOURCE_ASPECT:
        return VLC_SUCCESS;
    case VOUT_DISPLAY_CHANGE_DISPLAY_SIZE:
    {
        const vout_display_cfg_t *cfg = va_arg(args, const vout_display_cfg_t *);
        UpdateDisplaySize(vd, cfg);
        return VLC_SUCCESS;
    }
    case VOUT_DISPLAY_CHANGE_FULLSCREEN:
    case VOUT_DISPLAY_CHANGE_WINDOW_STATE:
    case VOUT_DISPLAY_CHANGE_DISPLAY_FILLED:
    case VOUT_DISPLAY_CHANGE_ZOOM:
    case VOUT_DISPLAY_CHANGE_SOURCE_CROP:
    case VOUT_DISPLAY_GET_OPENGL:
        return VLC_EGENERIC;
    }
}
