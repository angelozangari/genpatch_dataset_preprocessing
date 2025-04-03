 */
static void Close( vlc_object_t *p_this )
{
    video_splitter_t *p_splitter = (video_splitter_t*)p_this;
    video_splitter_sys_t *p_sys = p_splitter->p_sys;
    free( p_splitter->p_output );
    free( p_sys );
}
