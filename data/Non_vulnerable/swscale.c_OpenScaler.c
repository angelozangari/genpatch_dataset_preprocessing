 *****************************************************************************/
static int OpenScaler( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t*)p_this;
    filter_sys_t *p_sys;
    int i_sws_mode;
    if( GetParameters( NULL,
                       &p_filter->fmt_in.video,
                       &p_filter->fmt_out.video, 0 ) )
        return VLC_EGENERIC;
    /* */
    p_filter->pf_video_filter = Filter;
    /* Allocate the memory needed to store the decoder's structure */
    if( ( p_filter->p_sys = p_sys = malloc(sizeof(filter_sys_t)) ) == NULL )
        return VLC_ENOMEM;
    /* Set CPU capabilities */
    p_sys->i_cpu_mask = GetSwsCpuMask();
    /* */
    i_sws_mode = var_CreateGetInteger( p_filter, "swscale-mode" );
    switch( i_sws_mode )
    {
    case 0:  p_sys->i_sws_flags = SWS_FAST_BILINEAR; break;
    case 1:  p_sys->i_sws_flags = SWS_BILINEAR; break;
    case 2:  p_sys->i_sws_flags = SWS_BICUBIC; break;
    case 3:  p_sys->i_sws_flags = SWS_X; break;
    case 4:  p_sys->i_sws_flags = SWS_POINT; break;
    case 5:  p_sys->i_sws_flags = SWS_AREA; break;
    case 6:  p_sys->i_sws_flags = SWS_BICUBLIN; break;
    case 7:  p_sys->i_sws_flags = SWS_GAUSS; break;
    case 8:  p_sys->i_sws_flags = SWS_SINC; break;
    case 9:  p_sys->i_sws_flags = SWS_LANCZOS; break;
    case 10: p_sys->i_sws_flags = SWS_SPLINE; break;
    default: p_sys->i_sws_flags = SWS_BICUBIC; i_sws_mode = 2; break;
    }
    p_sys->p_src_filter = NULL;
    p_sys->p_dst_filter = NULL;
    /* Misc init */
    p_sys->ctx = NULL;
    p_sys->ctxA = NULL;
    p_sys->p_src_a = NULL;
    p_sys->p_dst_a = NULL;
    p_sys->p_src_e = NULL;
    p_sys->p_dst_e = NULL;
    memset( &p_sys->fmt_in,  0, sizeof(p_sys->fmt_in) );
    memset( &p_sys->fmt_out, 0, sizeof(p_sys->fmt_out) );
    if( Init( p_filter ) )
    {
        if( p_sys->p_src_filter )
            sws_freeFilter( p_sys->p_src_filter );
        free( p_sys );
        return VLC_EGENERIC;
    }
    msg_Dbg( p_filter, "%ix%i (%ix%i) chroma: %4.4s -> %ix%i (%ix%i) chroma: %4.4s with scaling using %s",
             p_filter->fmt_in.video.i_visible_width, p_filter->fmt_in.video.i_visible_height,
             p_filter->fmt_in.video.i_width, p_filter->fmt_in.video.i_height,
             (char *)&p_filter->fmt_in.video.i_chroma,
             p_filter->fmt_out.video.i_visible_width, p_filter->fmt_out.video.i_visible_height,
             p_filter->fmt_out.video.i_width, p_filter->fmt_out.video.i_height,
             (char *)&p_filter->fmt_out.video.i_chroma,
             ppsz_mode_descriptions[i_sws_mode] );
    return VLC_SUCCESS;
}
