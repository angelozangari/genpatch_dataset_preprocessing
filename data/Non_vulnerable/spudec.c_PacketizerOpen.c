 *****************************************************************************/
static int PacketizerOpen( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t*)p_this;
    if( DecoderOpen( p_this ) )
    {
        return VLC_EGENERIC;
    }
    p_dec->pf_packetize  = Packetize;
    p_dec->p_sys->b_packetizer = true;
    es_format_Copy( &p_dec->fmt_out, &p_dec->fmt_in );
    p_dec->fmt_out.i_codec = VLC_CODEC_SPU;
    return VLC_SUCCESS;
}
