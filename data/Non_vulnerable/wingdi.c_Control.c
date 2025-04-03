}
static int Control(vout_display_t *vd, int query, va_list args)
{
    switch (query) {
    case VOUT_DISPLAY_RESET_PICTURES:
        assert(0);
        return VLC_EGENERIC;
    default:
        return CommonControl(vd, query, args);
    }
}
