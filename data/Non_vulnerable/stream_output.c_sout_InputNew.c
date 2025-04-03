 *****************************************************************************/
sout_packetizer_input_t *sout_InputNew( sout_instance_t *p_sout,
                                        es_format_t *p_fmt )
{
    sout_packetizer_input_t *p_input;
    /* *** create a packetizer input *** */
    p_input         = malloc( sizeof( sout_packetizer_input_t ) );
    if( !p_input )  return NULL;
    p_input->p_sout = p_sout;
    p_input->p_fmt  = p_fmt;
    msg_Dbg( p_sout, "adding a new sout input (sout_input:%p)", p_input );
    if( p_fmt->i_codec == VLC_CODEC_NULL )
    {
        vlc_object_release( p_sout );
        return p_input;
    }
    /* *** add it to the stream chain */
    vlc_mutex_lock( &p_sout->lock );
    p_input->id = p_sout->p_stream->pf_add( p_sout->p_stream, p_fmt );
    vlc_mutex_unlock( &p_sout->lock );
    if( p_input->id == NULL )
    {
        free( p_input );
        return NULL;
    }
    return( p_input );
}
