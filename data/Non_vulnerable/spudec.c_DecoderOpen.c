 *****************************************************************************/
static int DecoderOpen( vlc_object_t *p_this )
{
    decoder_t     *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys;
    if( p_dec->fmt_in.i_codec != VLC_CODEC_SPU )
        return VLC_EGENERIC;
    p_dec->p_sys = p_sys = malloc( sizeof( decoder_sys_t ) );
    p_sys->b_packetizer = false;
    p_sys->b_disabletrans = var_InheritBool( p_dec, "dvdsub-transparency" );
    p_sys->i_spu_size = 0;
    p_sys->i_spu      = 0;
    p_sys->p_block    = NULL;
    es_format_Init( &p_dec->fmt_out, SPU_ES, VLC_CODEC_SPU );
    p_dec->pf_decode_sub = Decode;
    p_dec->pf_packetize  = NULL;
    return VLC_SUCCESS;
}
