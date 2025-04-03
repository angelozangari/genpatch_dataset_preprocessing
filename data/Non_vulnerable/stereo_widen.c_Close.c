 *****************************************************************************/
static void Close( vlc_object_t *obj )
{
    filter_t *p_filter  = (filter_t *)obj;
    filter_sys_t *p_sys = p_filter->p_sys;
#define DEL_VAR(var) \
    var_DelCallback( p_filter, var, paramCallback, p_sys ); \
    var_Destroy( p_filter, var );
    DEL_VAR( "feedback" );
    DEL_VAR( "crossfeed" );
    DEL_VAR( "dry-mix" );
    var_Destroy( p_filter, "delay" );
    free( p_sys->pf_ringbuf );
    free( p_sys );
}
