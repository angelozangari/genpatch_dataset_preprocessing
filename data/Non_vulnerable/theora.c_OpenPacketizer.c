}
static int OpenPacketizer( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t*)p_this;
    int i_ret = OpenDecoder( p_this );
    if( i_ret == VLC_SUCCESS )
    {
        p_dec->p_sys->b_packetizer = true;
        p_dec->fmt_out.i_codec = VLC_CODEC_THEORA;
    }
    return i_ret;
}
