}
static int GetParameters( ScalerConfiguration *p_cfg,
                          const video_format_t *p_fmti,
                          const video_format_t *p_fmto,
                          int i_sws_flags_default )
{
    int i_fmti = -1;
    int i_fmto = -1;
    bool b_has_ai = false;
    bool b_has_ao = false;
    int i_sws_flags = i_sws_flags_default;
    bool b_swap_uvi = false;
    bool b_swap_uvo = false;
    GetFfmpegChroma( &i_fmti, p_fmti );
    GetFfmpegChroma( &i_fmto, p_fmto );
    if( p_fmti->i_chroma == p_fmto->i_chroma )
    {
        if( p_fmti->i_chroma == VLC_CODEC_YUVP && ALLOW_YUVP )
        {
            i_fmti = i_fmto = AV_PIX_FMT_GRAY8;
            i_sws_flags = SWS_POINT;
        }
    }
    FixParameters( &i_fmti, &b_has_ai, &b_swap_uvi, p_fmti->i_chroma );
    FixParameters( &i_fmto, &b_has_ao, &b_swap_uvo, p_fmto->i_chroma );
#if !defined (__ANDROID__) && !defined(TARGET_OS_IPHONE)
    /* FIXME TODO removed when ffmpeg is fixed
     * Without SWS_ACCURATE_RND the quality is really bad for some conversions */
    switch( i_fmto )
    {
    case AV_PIX_FMT_ARGB:
    case AV_PIX_FMT_RGBA:
    case AV_PIX_FMT_ABGR:
        i_sws_flags |= SWS_ACCURATE_RND;
        break;
    }
#endif
    if( p_cfg )
    {
        p_cfg->i_fmti = i_fmti;
        p_cfg->i_fmto = i_fmto;
        p_cfg->b_has_a = b_has_ai && b_has_ao;
        p_cfg->b_add_a = (!b_has_ai) && b_has_ao;
        p_cfg->b_copy = i_fmti == i_fmto &&
                        p_fmti->i_visible_width == p_fmto->i_visible_width &&
                        p_fmti->i_visible_height == p_fmto->i_visible_height;
        p_cfg->b_swap_uvi = b_swap_uvi;
        p_cfg->b_swap_uvo = b_swap_uvo;
        p_cfg->i_sws_flags = i_sws_flags;
    }
    if( i_fmti < 0 || i_fmto < 0 )
        return VLC_EGENERIC;
    return VLC_SUCCESS;
}
