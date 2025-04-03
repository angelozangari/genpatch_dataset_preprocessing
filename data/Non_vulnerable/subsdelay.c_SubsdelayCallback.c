 *****************************************************************************/
static int SubsdelayCallback( vlc_object_t *p_this, char const *psz_var, vlc_value_t oldval, vlc_value_t newval,
        void *p_data )
{
    filter_sys_t *p_sys = (filter_sys_t *) p_data;
    VLC_UNUSED( oldval );
    SubsdelayHeapLock( &p_sys->heap );
    if( !strcmp( psz_var, CFG_MODE ) )
    {
        p_sys->i_mode = newval.i_int;
    }
    else if( !strcmp( psz_var, CFG_FACTOR ) )
    {
        p_sys->i_factor = FLOAT_FACTOR_TO_INT_FACTOR( newval.f_float );
    }
    else if( !strcmp( psz_var, CFG_OVERLAP ) )
    {
        p_sys->i_overlap = newval.i_int;
    }
    else if( !strcmp( psz_var, CFG_MIN_ALPHA ) )
    {
        p_sys->i_min_alpha = newval.i_int;
    }
    else if( !strcmp( psz_var, CFG_MIN_STOPS_INTERVAL ) )
    {
        p_sys->i_min_stops_interval = MILLISEC_TO_MICROSEC( newval.i_int );
    }
    else if( !strcmp( psz_var, CFG_MIN_STOP_START_INTERVAL ) )
    {
        p_sys->i_min_stop_start_interval = MILLISEC_TO_MICROSEC( newval.i_int );
    }
    else if( !strcmp( psz_var, CFG_MIN_START_STOP_INTERVAL ) )
    {
        p_sys->i_min_start_stop_interval = MILLISEC_TO_MICROSEC( newval.i_int );
    }
    else
    {
        SubsdelayHeapUnlock( &p_sys->heap );
        return VLC_ENOVAR;
    }
    SubsdelayRecalculateDelays( (filter_t *) p_this );
    SubsdelayHeapUnlock( &p_sys->heap );
    return VLC_SUCCESS;
}
