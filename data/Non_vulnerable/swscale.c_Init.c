}
static int Init( filter_t *p_filter )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    const video_format_t *p_fmti = &p_filter->fmt_in.video;
    video_format_t       *p_fmto = &p_filter->fmt_out.video;
    if( p_fmti->orientation != p_fmto->orientation )
        return VLC_EGENERIC;
    if( video_format_IsSimilar( p_fmti, &p_sys->fmt_in ) &&
        video_format_IsSimilar( p_fmto, &p_sys->fmt_out ) &&
        p_sys->ctx )
    {
        return VLC_SUCCESS;
    }
    Clean( p_filter );
    /* Init with new parameters */
    ScalerConfiguration cfg;
    if( GetParameters( &cfg, p_fmti, p_fmto, p_sys->i_sws_flags ) )
    {
        msg_Err( p_filter, "format not supported" );
        return VLC_EGENERIC;
    }
    if( p_fmti->i_visible_width <= 0 || p_fmti->i_visible_height <= 0 ||
        p_fmto->i_visible_width <= 0 || p_fmto->i_visible_height <= 0 )
    {
        msg_Err( p_filter, "invalid scaling: %ix%i -> %ix%i",
                 p_fmti->i_visible_width, p_fmti->i_visible_height,
                 p_fmto->i_visible_width, p_fmto->i_visible_height);
        return VLC_EGENERIC;
    }
    p_sys->desc_in = vlc_fourcc_GetChromaDescription( p_fmti->i_chroma );
    p_sys->desc_out = vlc_fourcc_GetChromaDescription( p_fmto->i_chroma );
    if( p_sys->desc_in == NULL || p_sys->desc_out == NULL )
        return VLC_EGENERIC;
    /* swscale does not like too small width */
    p_sys->i_extend_factor = 1;
    while( __MIN( p_fmti->i_visible_width, p_fmto->i_visible_width ) * p_sys->i_extend_factor < MINIMUM_WIDTH)
        p_sys->i_extend_factor++;
    const unsigned i_fmti_visible_width = p_fmti->i_visible_width * p_sys->i_extend_factor;
    const unsigned i_fmto_visible_width = p_fmto->i_visible_width * p_sys->i_extend_factor;
    for( int n = 0; n < (cfg.b_has_a ? 2 : 1); n++ )
    {
        const int i_fmti = n == 0 ? cfg.i_fmti : AV_PIX_FMT_GRAY8;
        const int i_fmto = n == 0 ? cfg.i_fmto : AV_PIX_FMT_GRAY8;
        struct SwsContext *ctx;
        ctx = sws_getContext( i_fmti_visible_width, p_fmti->i_visible_height, i_fmti,
                              i_fmto_visible_width, p_fmto->i_visible_height, i_fmto,
                              cfg.i_sws_flags | p_sys->i_cpu_mask,
                              p_sys->p_src_filter, p_sys->p_dst_filter, 0 );
        if( n == 0 )
            p_sys->ctx = ctx;
        else
            p_sys->ctxA = ctx;
    }
    if( p_sys->ctxA )
    {
        p_sys->p_src_a = picture_New( VLC_CODEC_GREY, i_fmti_visible_width, p_fmti->i_visible_height, 0, 1 );
        p_sys->p_dst_a = picture_New( VLC_CODEC_GREY, i_fmto_visible_width, p_fmto->i_visible_height, 0, 1 );
    }
    if( p_sys->i_extend_factor != 1 )
    {
        p_sys->p_src_e = picture_New( p_fmti->i_chroma, i_fmti_visible_width, p_fmti->i_visible_height, 0, 1 );
        p_sys->p_dst_e = picture_New( p_fmto->i_chroma, i_fmto_visible_width, p_fmto->i_visible_height, 0, 1 );
        if( p_sys->p_src_e )
            memset( p_sys->p_src_e->p[0].p_pixels, 0, p_sys->p_src_e->p[0].i_pitch * p_sys->p_src_e->p[0].i_lines );
        if( p_sys->p_dst_e )
            memset( p_sys->p_dst_e->p[0].p_pixels, 0, p_sys->p_dst_e->p[0].i_pitch * p_sys->p_dst_e->p[0].i_lines );
    }
    if( !p_sys->ctx ||
        ( cfg.b_has_a && ( !p_sys->ctxA || !p_sys->p_src_a || !p_sys->p_dst_a ) ) ||
        ( p_sys->i_extend_factor != 1 && ( !p_sys->p_src_e || !p_sys->p_dst_e ) ) )
    {
        msg_Err( p_filter, "could not init SwScaler and/or allocate memory" );
        Clean( p_filter );
        return VLC_EGENERIC;
    }
    if (p_filter->b_allow_fmt_out_change)
    {
        /*
         * If the transformation is not homothetic we must modify the
         * aspect ratio of the output format in order to have the
         * output picture displayed correctly and not stretched
         * horizontally or vertically.
         * WARNING: this is a hack, ideally this should not be needed
         * and the vout should update its video format instead.
         */
        unsigned i_sar_num = p_fmti->i_sar_num * p_fmti->i_visible_width;
        unsigned i_sar_den = p_fmti->i_sar_den * p_fmto->i_visible_width;
        vlc_ureduce(&i_sar_num, &i_sar_den, i_sar_num, i_sar_den, 65536);
        i_sar_num *= p_fmto->i_visible_height;
        i_sar_den *= p_fmti->i_visible_height;
        vlc_ureduce(&i_sar_num, &i_sar_den, i_sar_num, i_sar_den, 65536);
        p_fmto->i_sar_num = i_sar_num;
        p_fmto->i_sar_den = i_sar_den;
    }
    p_sys->b_add_a = cfg.b_add_a;
    p_sys->b_copy = cfg.b_copy;
    p_sys->fmt_in  = *p_fmti;
    p_sys->fmt_out = *p_fmto;
    p_sys->b_swap_uvi = cfg.b_swap_uvi;
    p_sys->b_swap_uvo = cfg.b_swap_uvo;
#if 0
    msg_Dbg( p_filter, "%ix%i (%ix%i) chroma: %4.4s -> %ix%i (%ix%i) chroma: %4.4s extend by %d",
             p_fmti->i_visible_width, p_fmti->i_visible_height, p_fmti->i_width, p_fmti->i_height, (char *)&p_fmti->i_chroma,
             p_fmto->i_visible_width, p_fmto->i_visible_height, p_fmto->i_width, p_fmto->i_height, (char *)&p_fmto->i_chroma,
             p_sys->i_extend_factor );
#endif
    return VLC_SUCCESS;
}
