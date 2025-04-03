 *****************************************************************************/
static void Destroy( vlc_object_t *p_this )
{
    filter_t *p_filter = ( filter_t * )p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    free( p_sys->psz_template );
    free( p_sys );
    rsvg_term( );
}
