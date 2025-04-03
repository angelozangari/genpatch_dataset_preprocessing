/* Destroy a "stream_out" module */
static void sout_StreamDelete( sout_stream_t *p_stream )
{
    sout_instance_t *p_sout = (sout_instance_t *)(p_stream->p_parent);
    msg_Dbg( p_stream, "destroying chain... (name=%s)", p_stream->psz_name );
    p_sout->i_out_pace_nocontrol -= p_stream->pace_nocontrol;
    if( p_stream->p_module != NULL )
        module_unneed( p_stream, p_stream->p_module );
    FREENULL( p_stream->psz_name );
    config_ChainDestroy( p_stream->p_cfg );
    msg_Dbg( p_stream, "destroying chain done" );
    vlc_object_release( p_stream );
}
