 */
static int Open( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t*)p_this;
    filter_sys_t *p_sys;
    /* Assert video in match with video out */
    if( !es_format_IsSimilar( &p_filter->fmt_in, &p_filter->fmt_out ) ) {
        msg_Err( p_filter, "Input and output format does not match" );
        return VLC_EGENERIC;
    }
    /* Reject 0 bpp and unsupported chroma */
    const vlc_fourcc_t fourcc = p_filter->fmt_in.video.i_chroma;
    const vlc_chroma_description_t *p_chroma =
        vlc_fourcc_GetChromaDescription( p_filter->fmt_in.video.i_chroma );
    if( !p_chroma || p_chroma->pixel_size == 0
        || p_chroma->plane_count < 3 || p_chroma->pixel_size > 1
        || !vlc_fourcc_IsYUV( fourcc ) )
    {
        msg_Err( p_filter, "Unsupported chroma (%4.4s)", (char*)&fourcc );
        return VLC_EGENERIC;
    }
    /* Allocate structure */
    p_filter->p_sys = p_sys = calloc(1, sizeof(*p_sys) );
    if( unlikely( !p_sys ) )
        return VLC_ENOMEM;
    /* init data */
    p_filter->pf_video_filter = Filter;
    p_sys->i_start_time = p_sys->i_cur_time = p_sys->i_last_time = NTPtime64();
    return VLC_SUCCESS;
}
