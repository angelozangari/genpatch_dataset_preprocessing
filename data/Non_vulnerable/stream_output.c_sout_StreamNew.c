 */
static sout_stream_t *sout_StreamNew( sout_instance_t *p_sout, char *psz_name,
                               config_chain_t *p_cfg, sout_stream_t *p_next)
{
    sout_stream_t *p_stream;
    assert(psz_name);
    p_stream = vlc_custom_create( p_sout, sizeof( *p_stream ), "stream out" );
    if( !p_stream )
        return NULL;
    p_stream->p_sout   = p_sout;
    p_stream->psz_name = psz_name;
    p_stream->p_cfg    = p_cfg;
    p_stream->p_next   = p_next;
    p_stream->pace_nocontrol = false;
    p_stream->p_sys = NULL;
    msg_Dbg( p_sout, "stream=`%s'", p_stream->psz_name );
    p_stream->p_module =
        module_need( p_stream, "sout stream", p_stream->psz_name, true );
    if( !p_stream->p_module )
    {
        /* those must be freed by the caller if creation failed */
        p_stream->psz_name = NULL;
        p_stream->p_cfg = NULL;
        sout_StreamDelete( p_stream );
        return NULL;
    }
    p_sout->i_out_pace_nocontrol += p_stream->pace_nocontrol;
    return p_stream;
}
