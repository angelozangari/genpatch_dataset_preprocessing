{ 44100, 48000, 32000, 22050, 24000, 16000 };
static int OpenEncoder( vlc_object_t *p_this )
{
    encoder_t *p_enc = (encoder_t *)p_this;
    encoder_sys_t *p_sys;
    int i_frequency;
    if( p_enc->fmt_out.i_codec != VLC_CODEC_MP2 &&
        p_enc->fmt_out.i_codec != VLC_CODEC_MPGA &&
        p_enc->fmt_out.i_codec != VLC_FOURCC( 'm', 'p', '2', 'a' ) &&
        !p_enc->b_force )
    {
        return VLC_EGENERIC;
    }
    if( p_enc->fmt_in.audio.i_channels > 2 )
    {
        msg_Err( p_enc, "doesn't support > 2 channels" );
        return VLC_EGENERIC;
    }
    for ( i_frequency = 0; i_frequency < 6; i_frequency++ )
    {
        if ( p_enc->fmt_out.audio.i_rate == mpa_freq_tab[i_frequency] )
            break;
    }
    if ( i_frequency == 6 )
    {
        msg_Err( p_enc, "MPEG audio doesn't support frequency=%d",
                 p_enc->fmt_out.audio.i_rate );
        return VLC_EGENERIC;
    }
    /* Allocate the memory needed to store the decoder's structure */
    if( ( p_sys = (encoder_sys_t *)malloc(sizeof(encoder_sys_t)) ) == NULL )
        return VLC_ENOMEM;
    p_enc->p_sys = p_sys;
    p_enc->fmt_in.i_codec = VLC_CODEC_S16N;
    p_enc->fmt_out.i_cat = AUDIO_ES;
    p_enc->fmt_out.i_codec = VLC_CODEC_MPGA;
    config_ChainParse( p_enc, ENC_CFG_PREFIX, ppsz_enc_options, p_enc->p_cfg );
    p_sys->p_twolame = twolame_init();
    /* Set options */
    twolame_set_in_samplerate( p_sys->p_twolame, p_enc->fmt_out.audio.i_rate );
    twolame_set_out_samplerate( p_sys->p_twolame, p_enc->fmt_out.audio.i_rate );
    if( var_GetBool( p_enc, ENC_CFG_PREFIX "vbr" ) )
    {
        float f_quality = var_GetFloat( p_enc, ENC_CFG_PREFIX "quality" );
        if ( f_quality > 50.0 ) f_quality = 50.0;
        if ( f_quality < 0.0 ) f_quality = 0.0;
        twolame_set_VBR( p_sys->p_twolame, 1 );
        twolame_set_VBR_q( p_sys->p_twolame, f_quality );
    }
    else
    {
        int i;
        for ( i = 1; i < 14; i++ )
        {
            if ( p_enc->fmt_out.i_bitrate / 1000
                  <= mpa_bitrate_tab[i_frequency / 3][i] )
                break;
        }
        if ( p_enc->fmt_out.i_bitrate / 1000
              != mpa_bitrate_tab[i_frequency / 3][i] )
        {
            msg_Warn( p_enc, "MPEG audio doesn't support bitrate=%d, using %d",
                      p_enc->fmt_out.i_bitrate,
                      mpa_bitrate_tab[i_frequency / 3][i] * 1000 );
            p_enc->fmt_out.i_bitrate = mpa_bitrate_tab[i_frequency / 3][i]
                                        * 1000;
        }
        twolame_set_bitrate( p_sys->p_twolame,
                             p_enc->fmt_out.i_bitrate / 1000 );
    }
    if ( p_enc->fmt_in.audio.i_channels == 1 )
    {
        twolame_set_num_channels( p_sys->p_twolame, 1 );
        twolame_set_mode( p_sys->p_twolame, TWOLAME_MONO );
    }
    else
    {
        twolame_set_num_channels( p_sys->p_twolame, 2 );
        switch( var_GetInteger( p_enc, ENC_CFG_PREFIX "mode" ) )
        {
        case 1:
            twolame_set_mode( p_sys->p_twolame, TWOLAME_DUAL_CHANNEL );
            break;
        case 2:
            twolame_set_mode( p_sys->p_twolame, TWOLAME_JOINT_STEREO );
            break;
        case 0:
        default:
            twolame_set_mode( p_sys->p_twolame, TWOLAME_STEREO );
            break;
        }
    }
    twolame_set_psymodel( p_sys->p_twolame,
                          var_GetInteger( p_enc, ENC_CFG_PREFIX "psy" ) );
    if ( twolame_init_params( p_sys->p_twolame ) )
    {
        msg_Err( p_enc, "twolame initialization failed" );
        return -VLC_EGENERIC;
    }
    p_enc->pf_encode_audio = Encode;
    p_sys->i_nb_samples = 0;
    return VLC_SUCCESS;
}
