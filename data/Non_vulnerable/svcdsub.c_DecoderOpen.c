 *****************************************************************************/
static int DecoderOpen( vlc_object_t *p_this )
{
    decoder_t     *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys;
    if( p_dec->fmt_in.i_codec != VLC_CODEC_OGT )
        return VLC_EGENERIC;
    p_dec->p_sys = p_sys = calloc( 1, sizeof( decoder_sys_t ) );
    if( p_sys == NULL )
        return VLC_ENOMEM;
    p_sys->i_image = -1;
    p_sys->i_state = SUBTITLE_BLOCK_EMPTY;
    p_sys->p_spu   = NULL;
    es_format_Init( &p_dec->fmt_out, SPU_ES, VLC_CODEC_OGT );
    p_dec->pf_decode_sub = Decode;
    p_dec->pf_packetize  = Packetize;
    return VLC_SUCCESS;
}
