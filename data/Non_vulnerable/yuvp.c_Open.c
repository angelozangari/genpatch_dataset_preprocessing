 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t*)p_this;
    /* It only supports YUVP to YUVA/RGBA without scaling
     * (if scaling is required another filter can do it) */
    if( p_filter->fmt_in.video.i_chroma != VLC_CODEC_YUVP ||
        ( p_filter->fmt_out.video.i_chroma != VLC_CODEC_YUVA &&
          p_filter->fmt_out.video.i_chroma != VLC_CODEC_RGBA &&
          p_filter->fmt_out.video.i_chroma != VLC_CODEC_ARGB ) ||
        p_filter->fmt_in.video.i_width  != p_filter->fmt_out.video.i_width ||
        p_filter->fmt_in.video.i_height != p_filter->fmt_out.video.i_height ||
        p_filter->fmt_in.video.orientation != p_filter->fmt_out.video.orientation )
    {
        return VLC_EGENERIC;
    }
    p_filter->pf_video_filter = Filter;
    msg_Dbg( p_filter, "YUVP to %4.4s converter",
             (const char*)&p_filter->fmt_out.video.i_chroma );
    return VLC_SUCCESS;
}
