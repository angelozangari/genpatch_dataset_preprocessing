 *****************************************************************************/
static int Activate( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    if( p_filter->fmt_in.video.i_width & 1
     || p_filter->fmt_in.video.i_height & 1 )
    {
        return -1;
    }
    if( p_filter->fmt_in.video.i_width != p_filter->fmt_out.video.i_width
     || p_filter->fmt_in.video.i_height != p_filter->fmt_out.video.i_height
     || p_filter->fmt_in.video.orientation != p_filter->fmt_out.video.orientation)
        return -1;
    switch( p_filter->fmt_out.video.i_chroma )
    {
        case VLC_CODEC_I422:
            switch( p_filter->fmt_in.video.i_chroma )
            {
                case VLC_CODEC_YUYV:
                    p_filter->pf_video_filter = YUY2_I422_Filter;
                    break;
                case VLC_CODEC_YVYU:
                    p_filter->pf_video_filter = YVYU_I422_Filter;
                    break;
                case VLC_CODEC_UYVY:
                    p_filter->pf_video_filter = UYVY_I422_Filter;
                    break;
                case VLC_CODEC_CYUV:
                    p_filter->pf_video_filter = cyuv_I422_Filter;
                    break;
                default:
                    return -1;
            }
            break;
        default:
            return -1;
    }
    return 0;
}
