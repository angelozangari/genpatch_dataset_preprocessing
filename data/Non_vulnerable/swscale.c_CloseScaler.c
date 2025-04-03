 *****************************************************************************/
static void CloseScaler( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t*)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    Clean( p_filter );
    if( p_sys->p_src_filter )
        sws_freeFilter( p_sys->p_src_filter );
    free( p_sys );
}
