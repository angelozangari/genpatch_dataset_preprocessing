 *****************************************************************************/
int sout_InputSendBuffer( sout_packetizer_input_t *p_input,
                          block_t *p_buffer )
{
    sout_instance_t     *p_sout = p_input->p_sout;
    int                 i_ret;
    if( p_input->p_fmt->i_codec == VLC_CODEC_NULL )
    {
        block_Release( p_buffer );
        return VLC_SUCCESS;
    }
    if( p_buffer->i_dts <= VLC_TS_INVALID )
    {
        msg_Warn( p_sout, "trying to send non-dated packet to stream output!");
        block_Release( p_buffer );
        return VLC_SUCCESS;
    }
    vlc_mutex_lock( &p_sout->lock );
    i_ret = p_sout->p_stream->pf_send( p_sout->p_stream,
                                       p_input->id, p_buffer );
    vlc_mutex_unlock( &p_sout->lock );
    return i_ret;
}
