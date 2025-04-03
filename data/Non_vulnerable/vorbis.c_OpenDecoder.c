 *****************************************************************************/
static int OpenDecoder( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys;
    if( p_dec->fmt_in.i_codec != VLC_CODEC_VORBIS )
        return VLC_EGENERIC;
    /* Allocate the memory needed to store the decoder's structure */
    p_dec->p_sys = p_sys = malloc( sizeof(*p_sys) );
    if( unlikely( !p_sys ) )
        return VLC_ENOMEM;
    /* Misc init */
    date_Set( &p_sys->end_date, 0 );
    p_sys->i_last_block_size = 0;
    p_sys->b_packetizer = false;
    p_sys->b_has_headers = false;
    /* Take care of vorbis init */
    vorbis_info_init( &p_sys->vi );
    vorbis_comment_init( &p_sys->vc );
    /* Set output properties */
    p_dec->fmt_out.i_cat = AUDIO_ES;
#ifdef MODULE_NAME_IS_tremor
    p_dec->fmt_out.i_codec = VLC_CODEC_S32N;
#else
    p_dec->fmt_out.i_codec = VLC_CODEC_FL32;
#endif
    /* Set callbacks */
    p_dec->pf_decode_audio = DecodeBlock;
    p_dec->pf_packetize    = DecodeBlock;
    return VLC_SUCCESS;
}
