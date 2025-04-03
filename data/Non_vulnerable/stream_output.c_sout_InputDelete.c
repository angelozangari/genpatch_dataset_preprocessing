 *****************************************************************************/
int sout_InputDelete( sout_packetizer_input_t *p_input )
{
    sout_instance_t     *p_sout = p_input->p_sout;
    msg_Dbg( p_sout, "removing a sout input (sout_input:%p)", p_input );
    if( p_input->p_fmt->i_codec != VLC_CODEC_NULL )
    {
        vlc_mutex_lock( &p_sout->lock );
        p_sout->p_stream->pf_del( p_sout->p_stream, p_input->id );
        vlc_mutex_unlock( &p_sout->lock );
    }
    free( p_input );
    return( VLC_SUCCESS);
}
