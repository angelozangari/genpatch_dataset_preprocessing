 *****************************************************************************/
static int ActiveKeyCallback( vlc_object_t *p_this, char const *psz_cmd,
                           vlc_value_t oldval, vlc_value_t newval,
                           void *p_data )
{
    VLC_UNUSED(psz_cmd); VLC_UNUSED(oldval); VLC_UNUSED(p_data);
    sout_mux_t      *p_mux = (sout_mux_t*)p_this;
    sout_mux_sys_t  *p_sys = p_mux->p_sys;
    int             i_res, use_odd = -1;
    if( !strcmp(newval.psz_string, "odd" ) ||
        !strcmp(newval.psz_string, "first" ) ||
        !strcmp(newval.psz_string, "1" ) )
    {
        use_odd = 1;
    }
    else if( !strcmp(newval.psz_string, "even" ) ||
             !strcmp(newval.psz_string, "second" ) ||
             !strcmp(newval.psz_string, "2" ) )
    {
        use_odd = 0;
    }
    if (use_odd < 0)
        return VLC_EBADVAR;
    vlc_mutex_lock( &p_sys->csa_lock );
    i_res = csa_UseKey( p_this, p_sys->csa, use_odd );
    vlc_mutex_unlock( &p_sys->csa_lock );
    return i_res;
}
