}
static int SubMarginCallback( vlc_object_t *p_this, char const *psz_cmd,
                              vlc_value_t oldval, vlc_value_t newval, void *p_data)
{
    vout_thread_t *p_vout = (vout_thread_t *)p_this;
    VLC_UNUSED(psz_cmd); VLC_UNUSED(oldval); VLC_UNUSED(p_data);
    vout_ControlChangeSubMargin( p_vout, newval.i_int );
    return VLC_SUCCESS;
}
