}
static int Send( sout_stream_t *p_stream, sout_stream_id_sys_t *id,
                 block_t *p_buffer )
{
    return sout_MuxSendBuffer( p_stream->p_sys->p_mux, (sout_input_t*)id, p_buffer );
}
