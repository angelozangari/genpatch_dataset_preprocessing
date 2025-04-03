 *****************************************************************************/
static int OpenEncoder( vlc_object_t *p_this )
{
    encoder_t *p_enc = (encoder_t *)p_this;
    encoder_sys_t *p_sys;
    int i_quality;
    int t_flags;
    int max_enc_level = 0;
    int keyframe_freq_force = 64;
    ogg_packet header;
    int status;
    if( p_enc->fmt_out.i_codec != VLC_CODEC_THEORA &&
        !p_enc->b_force )
    {
        return VLC_EGENERIC;
    }
    /* Allocate the memory needed to store the decoder's structure */
    if( ( p_sys = malloc(sizeof(encoder_sys_t)) ) == NULL )
        return VLC_ENOMEM;
    p_enc->p_sys = p_sys;
    p_enc->pf_encode_video = Encode;
    p_enc->fmt_in.i_codec = VLC_CODEC_I420;
    p_enc->fmt_out.i_codec = VLC_CODEC_THEORA;
    config_ChainParse( p_enc, ENC_CFG_PREFIX, ppsz_enc_options, p_enc->p_cfg );
    i_quality = var_GetInteger( p_enc, ENC_CFG_PREFIX "quality" );
    if( i_quality > 10 ) i_quality = 10;
    if( i_quality < 0 ) i_quality = 0;
    th_info_init( &p_sys->ti );
    p_sys->ti.frame_width = p_enc->fmt_in.video.i_visible_width;
    p_sys->ti.frame_height = p_enc->fmt_in.video.i_visible_height;
    if( p_sys->ti.frame_width % 16 || p_sys->ti.frame_height % 16 )
    {
        /* Pictures from the transcoder should always have a pitch
         * which is a multiple of 16 */
        p_sys->ti.frame_width = (p_sys->ti.frame_width + 15) >> 4 << 4;
        p_sys->ti.frame_height = (p_sys->ti.frame_height + 15) >> 4 << 4;
        msg_Dbg( p_enc, "padding video from %dx%d to %dx%d",
                 p_enc->fmt_in.video.i_visible_width, p_enc->fmt_in.video.i_visible_height,
                 p_sys->ti.frame_width, p_sys->ti.frame_height );
    }
    p_sys->ti.pic_width = p_enc->fmt_in.video.i_visible_width;
    p_sys->ti.pic_height = p_enc->fmt_in.video.i_visible_height;
    p_sys->ti.pic_x = 0 /*frame_x_offset*/;
    p_sys->ti.pic_y = 0 /*frame_y_offset*/;
    p_sys->i_width = p_sys->ti.frame_width;
    p_sys->i_height = p_sys->ti.frame_height;
    if( !p_enc->fmt_in.video.i_frame_rate ||
        !p_enc->fmt_in.video.i_frame_rate_base )
    {
        p_sys->ti.fps_numerator = 25;
        p_sys->ti.fps_denominator = 1;
    }
    else
    {
        p_sys->ti.fps_numerator = p_enc->fmt_in.video.i_frame_rate;
        p_sys->ti.fps_denominator = p_enc->fmt_in.video.i_frame_rate_base;
    }
    if( p_enc->fmt_in.video.i_sar_num > 0 && p_enc->fmt_in.video.i_sar_den > 0 )
    {
        unsigned i_dst_num, i_dst_den;
        vlc_ureduce( &i_dst_num, &i_dst_den,
                     p_enc->fmt_in.video.i_sar_num,
                     p_enc->fmt_in.video.i_sar_den, 0 );
        p_sys->ti.aspect_numerator = i_dst_num;
        p_sys->ti.aspect_denominator = i_dst_den;
    }
    else
    {
        p_sys->ti.aspect_numerator = 4;
        p_sys->ti.aspect_denominator = 3;
    }
    p_sys->ti.target_bitrate = p_enc->fmt_out.i_bitrate;
    p_sys->ti.quality = ((float)i_quality) * 6.3;
    p_sys->tcx = th_encode_alloc( &p_sys->ti );
    th_comment_init( &p_sys->tc );
    /* These are no longer supported here: */
    /*
    p_sys->ti.dropframes_p = 0;
    p_sys->ti.quick_p = 1;
    p_sys->ti.keyframe_auto_p = 1;
    p_sys->ti.keyframe_frequency = 64;
    p_sys->ti.keyframe_frequency_force = 64;
    p_sys->ti.keyframe_data_target_bitrate = p_enc->fmt_out.i_bitrate * 1.5;
    p_sys->ti.keyframe_auto_threshold = 80;
    p_sys->ti.keyframe_mindistance = 8;
    p_sys->ti.noise_sensitivity = 1;
    */
    t_flags = TH_RATECTL_CAP_OVERFLOW; /* default is TH_RATECTL_CAP_OVERFLOW | TL_RATECTL_DROP_FRAMES */
    /* Turn off dropframes */
    th_encode_ctl( p_sys->tcx, TH_ENCCTL_SET_RATE_FLAGS, &t_flags, sizeof(t_flags) );
    /* turn on fast encoding */
    if ( !th_encode_ctl( p_sys->tcx, TH_ENCCTL_GET_SPLEVEL_MAX, &max_enc_level,
                sizeof(max_enc_level) ) ) /* returns 0 on success */
        th_encode_ctl( p_sys->tcx, TH_ENCCTL_SET_SPLEVEL, &max_enc_level, sizeof(max_enc_level) );
    /* Set forced distance between key frames */
    th_encode_ctl( p_sys->tcx, TH_ENCCTL_SET_KEYFRAME_FREQUENCY_FORCE,
                   &keyframe_freq_force, sizeof(keyframe_freq_force) );
    /* Create and store headers */
    while ( ( status = th_encode_flushheader( p_sys->tcx, &p_sys->tc, &header ) ) )
    {
        if ( status < 0 ) return VLC_EGENERIC;
        if( xiph_AppendHeaders( &p_enc->fmt_out.i_extra, &p_enc->fmt_out.p_extra,
                                header.bytes, header.packet ) )
        {
            p_enc->fmt_out.i_extra = 0;
            p_enc->fmt_out.p_extra = NULL;
        }
    }
    return VLC_SUCCESS;
}
