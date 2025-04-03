 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    decoder_t     *p_dec = (decoder_t *) p_this;
    if( p_dec->fmt_in.i_codec != VLC_CODEC_TX3G )
        return VLC_EGENERIC;
    p_dec->pf_decode_sub = Decode;
    p_dec->fmt_out.i_cat = SPU_ES;
    p_dec->fmt_out.i_codec = 0;
    return VLC_SUCCESS;
}
