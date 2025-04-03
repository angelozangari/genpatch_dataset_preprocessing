}
static int FullscreenCallback( vlc_object_t *p_this, char const *psz_cmd,
                       vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    vout_thread_t *p_vout = (vout_thread_t *)p_this;
    (void)psz_cmd; (void)p_data;
    if( oldval.b_bool != newval.b_bool )
        vout_ControlChangeFullscreen( p_vout, newval.b_bool );
    return VLC_SUCCESS;
}
