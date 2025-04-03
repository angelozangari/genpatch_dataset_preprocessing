 *****************************************************************************/
static int ChangeKeyCallback( vlc_object_t *p_this, char const *psz_cmd,
                           vlc_value_t oldval, vlc_value_t newval,
                           void *p_data )
{
    VLC_UNUSED(psz_cmd); VLC_UNUSED(oldval);
    sout_mux_t      *p_mux = (sout_mux_t*)p_this;
    sout_mux_sys_t  *p_sys = p_mux->p_sys;
    int ret;
    vlc_mutex_lock(&p_sys->csa_lock);
    ret = csa_SetCW(p_this, p_sys->csa, newval.psz_string, !!(intptr_t)p_data);
    vlc_mutex_unlock(&p_sys->csa_lock);
    return ret;
}
