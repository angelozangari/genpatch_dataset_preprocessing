 *****************************************************************************/
static void Close( vlc_object_t * p_this )
{
    access_t *p_access = (access_t*)p_this;
    access_sys_t *p_sys = p_access->p_sys;
    if( p_sys->fd != -1 )
        close( p_sys->fd );
    ARRAY_RESET( p_sys->file_sizes );
    if( p_sys->p_meta )
        vlc_meta_Delete( p_sys->p_meta );
    vlc_input_title_Delete( p_sys->p_marks );
    free( p_sys );
}
