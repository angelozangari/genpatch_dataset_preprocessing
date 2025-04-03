};
static sout_stream_id_sys_t * Add( sout_stream_t *p_stream, es_format_t *p_fmt )
{
    return (sout_stream_id_sys_t*)sout_MuxAddStream( p_stream->p_sys->p_mux, p_fmt );
}
