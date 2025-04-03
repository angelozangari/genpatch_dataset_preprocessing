 **********************************************************************/
static int paramCallback( vlc_object_t *p_this, char const *psz_var,
                            vlc_value_t oldval, vlc_value_t newval,
                            void *p_data )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = (filter_sys_t *) p_data;
    VLC_UNUSED(oldval);
    VLC_UNUSED(p_this);
    if( !strcmp( psz_var, "delay" ) )
    {
        if( MakeRingBuffer( &p_sys->pf_ringbuf, &p_sys->i_len, &p_sys->pf_write,
                            newval.f_float, p_filter->fmt_in.audio.i_rate ) != VLC_SUCCESS )
        {
            msg_Dbg( p_filter, "Couldnt allocate buffer for delay" );
        }
        else
        {
            p_sys->f_delay = newval.f_float;
        }
    }
    else if( !strcmp( psz_var, "feedback" ) )
        p_sys->f_feedback = newval.f_float;
    else if( !strcmp( psz_var, "crossfeed" ) )
        p_sys->f_feedback = newval.f_float;
    else if( !strcmp( psz_var, "dry-mix" ) )
        p_sys->f_dry_mix = newval.f_float;
    return VLC_SUCCESS;
}
