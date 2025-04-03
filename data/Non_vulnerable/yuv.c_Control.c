}
static int Control(vout_display_t *vd, int query, va_list args)
{
    VLC_UNUSED(vd);
    switch (query) {
    case VOUT_DISPLAY_CHANGE_FULLSCREEN: {
        const vout_display_cfg_t *cfg = va_arg(args, const vout_display_cfg_t *);
        if (cfg->is_fullscreen)
            return VLC_EGENERIC;
        return VLC_SUCCESS;
    }
    default:
        return VLC_EGENERIC;
    }
}
