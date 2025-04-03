 */
static void Close( vlc_object_t *p_this ) {
    filter_t *p_filter = (filter_t*)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    /* Free allocated memory */
    vhs_free_allocated_data( p_filter );
    free( p_sys );
}
