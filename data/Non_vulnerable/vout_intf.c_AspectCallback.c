}
static int AspectCallback( vlc_object_t *object, char const *cmd,
                         vlc_value_t oldval, vlc_value_t newval, void *data )
{
    vout_thread_t *vout = (vout_thread_t *)object;
    VLC_UNUSED(cmd); VLC_UNUSED(oldval); VLC_UNUSED(data);
    unsigned num, den;
    if (sscanf(newval.psz_string, "%u:%u", &num, &den) == 2 &&
        (num > 0) == (den > 0))
        vout_ControlChangeSampleAspectRatio(vout, num, den);
    else if (*newval.psz_string == '\0')
        vout_ControlChangeSampleAspectRatio(vout, 0, 0);
    return VLC_SUCCESS;
}
