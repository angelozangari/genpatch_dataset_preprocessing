 *****************************************************************************/
static int OpenEncoder( vlc_object_t *p_this )
{
    encoder_t *p_enc = (encoder_t *)p_this;
    encoder_sys_t *p_sys;
    int i_quality, i_min_bitrate, i_max_bitrate;
    ogg_packet header[3];
    if( p_enc->fmt_out.i_codec != VLC_CODEC_VORBIS &&
        !p_enc->b_force )
    {
        return VLC_EGENERIC;
    }
    /* Allocate the memory needed to store the decoder's structure */
    if( ( p_sys = (encoder_sys_t *)malloc(sizeof(encoder_sys_t)) ) == NULL )
        return VLC_ENOMEM;
    p_enc->p_sys = p_sys;
    p_enc->pf_encode_audio = Encode;
    p_enc->fmt_in.i_codec  = VLC_CODEC_FL32;
    p_enc->fmt_out.i_codec = VLC_CODEC_VORBIS;
    config_ChainParse( p_enc, ENC_CFG_PREFIX, ppsz_enc_options, p_enc->p_cfg );
    i_quality = var_GetInteger( p_enc, ENC_CFG_PREFIX "quality" );
    if( i_quality > 10 ) i_quality = 10;
    if( i_quality < 0 ) i_quality = 0;
    if( var_GetBool( p_enc, ENC_CFG_PREFIX "cbr" ) ) i_quality = 0;
    i_max_bitrate = var_GetInteger( p_enc, ENC_CFG_PREFIX "max-bitrate" );
    i_min_bitrate = var_GetInteger( p_enc, ENC_CFG_PREFIX "min-bitrate" );
    /* Initialize vorbis encoder */
    vorbis_info_init( &p_sys->vi );
    if( i_quality > 0 )
    {
        /* VBR mode */
        if( vorbis_encode_setup_vbr( &p_sys->vi,
              p_enc->fmt_in.audio.i_channels, p_enc->fmt_in.audio.i_rate,
              i_quality * 0.1 ) )
        {
            vorbis_info_clear( &p_sys->vi );
            free( p_enc->p_sys );
            msg_Err( p_enc, "VBR mode initialisation failed" );
            return VLC_EGENERIC;
        }
        /* Do we have optional hard quality restrictions? */
        if( i_max_bitrate > 0 || i_min_bitrate > 0 )
        {
            struct ovectl_ratemanage_arg ai;
            vorbis_encode_ctl( &p_sys->vi, OV_ECTL_RATEMANAGE_GET, &ai );
            ai.bitrate_hard_min = i_min_bitrate;
            ai.bitrate_hard_max = i_max_bitrate;
            ai.management_active = 1;
            vorbis_encode_ctl( &p_sys->vi, OV_ECTL_RATEMANAGE_SET, &ai );
        }
        else
        {
            /* Turn off management entirely */
            vorbis_encode_ctl( &p_sys->vi, OV_ECTL_RATEMANAGE_SET, NULL );
        }
    }
    else
    {
        if( vorbis_encode_setup_managed( &p_sys->vi,
              p_enc->fmt_in.audio.i_channels, p_enc->fmt_in.audio.i_rate,
              i_min_bitrate > 0 ? i_min_bitrate * 1000: -1,
              p_enc->fmt_out.i_bitrate,
              i_max_bitrate > 0 ? i_max_bitrate * 1000: -1 ) )
          {
              vorbis_info_clear( &p_sys->vi );
              msg_Err( p_enc, "CBR mode initialisation failed" );
              free( p_enc->p_sys );
              return VLC_EGENERIC;
          }
    }
    vorbis_encode_setup_init( &p_sys->vi );
    /* Add a comment */
    vorbis_comment_init( &p_sys->vc);
    vorbis_comment_add_tag( &p_sys->vc, "ENCODER", "VLC media player");
    /* Set up the analysis state and auxiliary encoding storage */
    vorbis_analysis_init( &p_sys->vd, &p_sys->vi );
    vorbis_block_init( &p_sys->vd, &p_sys->vb );
    /* Create and store headers */
    vorbis_analysis_headerout( &p_sys->vd, &p_sys->vc,
                               &header[0], &header[1], &header[2]);
    for( int i = 0; i < 3; i++ )
    {
        if( xiph_AppendHeaders( &p_enc->fmt_out.i_extra, &p_enc->fmt_out.p_extra,
                                header[i].bytes, header[i].packet ) )
        {
            p_enc->fmt_out.i_extra = 0;
            p_enc->fmt_out.p_extra = NULL;
        }
    }
    p_sys->i_channels = p_enc->fmt_in.audio.i_channels;
    p_sys->i_last_block_size = 0;
    p_sys->i_samples_delay = 0;
    ConfigureChannelOrder(p_sys->pi_chan_table, p_sys->vi.channels,
            p_enc->fmt_in.audio.i_physical_channels, true);
    return VLC_SUCCESS;
}
