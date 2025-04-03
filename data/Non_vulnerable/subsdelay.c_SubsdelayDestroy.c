 *****************************************************************************/
static void SubsdelayDestroy( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *) p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    SubsdelayHeapDestroy( &p_sys->heap );
    /* destroy parameters */
    var_DelCallback( p_filter, CFG_MODE, SubsdelayCallback, p_sys );
    var_Destroy( p_filter, CFG_MODE );
    var_DelCallback( p_filter, CFG_FACTOR, SubsdelayCallback, p_sys );
    var_Destroy( p_filter, CFG_FACTOR );
    var_DelCallback( p_filter, CFG_OVERLAP, SubsdelayCallback, p_sys );
    var_Destroy( p_filter, CFG_OVERLAP );
    var_DelCallback( p_filter, CFG_MIN_ALPHA, SubsdelayCallback, p_sys );
    var_Destroy( p_filter, CFG_MIN_ALPHA );
    var_DelCallback( p_filter, CFG_MIN_STOPS_INTERVAL, SubsdelayCallback, p_sys );
    var_Destroy( p_filter, CFG_MIN_STOPS_INTERVAL );
    var_DelCallback( p_filter, CFG_MIN_STOP_START_INTERVAL, SubsdelayCallback, p_sys );
    var_Destroy( p_filter, CFG_MIN_STOP_START_INTERVAL );
    var_DelCallback( p_filter, CFG_MIN_START_STOP_INTERVAL, SubsdelayCallback, p_sys );
    var_Destroy( p_filter, CFG_MIN_START_STOP_INTERVAL );
    free( p_sys );
}
