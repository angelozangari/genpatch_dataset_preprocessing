 *****************************************************************************/
static int OpenDecoder( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys;
    if( p_dec->fmt_in.i_codec != VLC_CODEC_WMA1 &&
        p_dec->fmt_in.i_codec != VLC_CODEC_WMA2 )
    {
        return VLC_EGENERIC;
    }
    /* Allocate the memory needed to store the decoder's structure */
    p_dec->p_sys = p_sys = (decoder_sys_t *)malloc(sizeof(decoder_sys_t));
    if( !p_sys )
        return VLC_ENOMEM;
    memset( p_sys, 0, sizeof( decoder_sys_t ) );
    /* Date */
    date_Init( &p_sys->end_date, p_dec->fmt_in.audio.i_rate, 1 );
    /* Set output properties */
    p_dec->fmt_out.i_cat = AUDIO_ES;
    p_dec->fmt_out.i_codec = VLC_CODEC_S32N;
    p_dec->fmt_out.audio.i_bitspersample = p_dec->fmt_in.audio.i_bitspersample;
    p_dec->fmt_out.audio.i_rate = p_dec->fmt_in.audio.i_rate;
    p_dec->fmt_out.audio.i_channels = p_dec->fmt_in.audio.i_channels;
    assert( p_dec->fmt_out.audio.i_channels <
        ( sizeof( pi_channels_maps ) / sizeof( pi_channels_maps[0] ) ) );
    p_dec->fmt_out.audio.i_original_channels =
        p_dec->fmt_out.audio.i_physical_channels =
            pi_channels_maps[p_dec->fmt_out.audio.i_channels];
    /* aout core assumes this number is not 0 and uses it in divisions */
    assert( p_dec->fmt_out.audio.i_physical_channels != 0 );
    asf_waveformatex_t wfx;
    wfx.rate = p_dec->fmt_in.audio.i_rate;
    wfx.bitrate = p_dec->fmt_in.i_bitrate;
    wfx.channels = p_dec->fmt_in.audio.i_channels;
    wfx.blockalign = p_dec->fmt_in.audio.i_blockalign;
    wfx.bitspersample = p_dec->fmt_in.audio.i_bitspersample;
    msg_Dbg( p_dec, "samplerate %d bitrate %d channels %d align %d bps %d",
        wfx.rate, wfx.bitrate, wfx.channels, wfx.blockalign,
        wfx.bitspersample );
    if( p_dec->fmt_in.i_codec == VLC_CODEC_WMA1 )
        wfx.codec_id = ASF_CODEC_ID_WMAV1;
    else if( p_dec->fmt_in.i_codec == VLC_CODEC_WMA2 )
        wfx.codec_id = ASF_CODEC_ID_WMAV2;
    wfx.datalen = p_dec->fmt_in.i_extra;
    if( wfx.datalen > 6 ) wfx.datalen = 6;
    if( wfx.datalen > 0 )
        memcpy( wfx.data, p_dec->fmt_in.p_extra, wfx.datalen );
    /* Init codec */
    if( wma_decode_init(&p_sys->wmadec, &wfx ) < 0 )
    {
        msg_Err( p_dec, "codec init failed" );
        free( p_sys );
        return VLC_EGENERIC;
    }
    /* Set callback */
    p_dec->pf_decode_audio = DecodeFrame;
    return VLC_SUCCESS;
}
