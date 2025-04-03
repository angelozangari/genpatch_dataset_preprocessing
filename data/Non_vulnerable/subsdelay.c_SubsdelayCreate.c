 *****************************************************************************/
static int SubsdelayCreate( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *) p_this;
    filter_sys_t *p_sys;
    /* allocate structure */
    p_sys = (filter_sys_t*) malloc( sizeof(filter_sys_t) );
    if( !p_sys )
    {
        return VLC_ENOMEM;
    }
    /* init parameters */
    p_sys->i_mode = var_CreateGetIntegerCommand( p_filter, CFG_MODE );
    var_AddCallback( p_filter, CFG_MODE, SubsdelayCallback, p_sys );
    p_sys->i_factor = FLOAT_FACTOR_TO_INT_FACTOR( var_CreateGetFloatCommand( p_filter, CFG_FACTOR ) );
    var_AddCallback( p_filter, CFG_FACTOR, SubsdelayCallback, p_sys );
    p_sys->i_overlap = var_CreateGetIntegerCommand( p_filter, CFG_OVERLAP );
    var_AddCallback( p_filter, CFG_OVERLAP, SubsdelayCallback, p_sys );
    p_sys->i_min_alpha = var_CreateGetIntegerCommand( p_filter, CFG_MIN_ALPHA );
    var_AddCallback( p_filter, CFG_MIN_ALPHA, SubsdelayCallback, p_sys );
    p_sys->i_min_stops_interval
            = MILLISEC_TO_MICROSEC( var_CreateGetIntegerCommand( p_filter, CFG_MIN_STOPS_INTERVAL ) );
    var_AddCallback( p_filter, CFG_MIN_STOPS_INTERVAL, SubsdelayCallback, p_sys );
    p_sys->i_min_stop_start_interval
            = MILLISEC_TO_MICROSEC( var_CreateGetIntegerCommand( p_filter, CFG_MIN_STOP_START_INTERVAL ) );
    var_AddCallback( p_filter, CFG_MIN_STOP_START_INTERVAL, SubsdelayCallback, p_sys );
    p_sys->i_min_start_stop_interval
            = MILLISEC_TO_MICROSEC( var_CreateGetIntegerCommand( p_filter, CFG_MIN_START_STOP_INTERVAL ) );
    var_AddCallback( p_filter, CFG_MIN_START_STOP_INTERVAL, SubsdelayCallback, p_sys );
    p_filter->p_sys = p_sys;
    p_filter->pf_sub_filter = SubsdelayFilter;
    config_ChainParse( p_filter, CFG_PREFIX, ppsz_filter_options, p_filter->p_cfg );
    SubsdelayHeapInit( &p_sys->heap );
    return VLC_SUCCESS;
}
