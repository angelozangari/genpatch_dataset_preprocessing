 *****************************************************************************/
static void Close( vlc_object_t * p_this )
{
    sout_stream_t     *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys    = p_stream->p_sys;
    sout_access_out_t *p_access = p_sys->p_mux->p_access;
    if( p_sys->p_session != NULL )
        sout_AnnounceUnRegister( p_stream, p_sys->p_session );
    sout_MuxDelete( p_sys->p_mux );
    sout_AccessOutDelete( p_access );
    free( p_sys );
}
