}
static int Del( sout_stream_t *p_stream, sout_stream_id_sys_t *id )
{
    sout_MuxDeleteStream( p_stream->p_sys->p_mux, (sout_input_t*)id );
    return VLC_SUCCESS;
}
