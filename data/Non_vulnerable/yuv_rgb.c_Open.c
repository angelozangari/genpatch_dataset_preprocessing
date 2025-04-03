VIDEO_FILTER_WRAPPER (NV12_RGBA)
static int Open (vlc_object_t *obj)
{
    filter_t *filter = (filter_t *)obj;
    if (!vlc_CPU_ARM_NEON())
        return VLC_EGENERIC;
    if (((filter->fmt_in.video.i_width | filter->fmt_in.video.i_height) & 1)
     || (filter->fmt_in.video.i_width != filter->fmt_out.video.i_width)
     || (filter->fmt_in.video.i_height != filter->fmt_out.video.i_height)
     || (filter->fmt_in.video.orientation != filter->fmt_out.video.orientation))
        return VLC_EGENERIC;
    switch (filter->fmt_out.video.i_chroma)
    {
        case VLC_CODEC_RGB16:
            switch (filter->fmt_in.video.i_chroma)
            {
                case VLC_CODEC_I420:
                    filter->pf_video_filter = I420_RV16_Filter;
                    break;
                default:
                    return VLC_EGENERIC;
            }
            break;
        case VLC_CODEC_RGB32:
            if(        filter->fmt_out.video.i_rmask != 0x000000ff
                    || filter->fmt_out.video.i_gmask != 0x0000ff00
                    || filter->fmt_out.video.i_bmask != 0x00ff0000 )
                return VLC_EGENERIC;
            switch (filter->fmt_in.video.i_chroma)
            {
                case VLC_CODEC_I420:
                    filter->pf_video_filter = I420_RGBA_Filter;
                    break;
                case VLC_CODEC_YV12:
                    filter->pf_video_filter = YV12_RGBA_Filter;
                    break;
                case VLC_CODEC_NV21:
                    filter->pf_video_filter = NV21_RGBA_Filter;
                    break;
                case VLC_CODEC_NV12:
                    filter->pf_video_filter = NV12_RGBA_Filter;
                    break;
                default:
                    return VLC_EGENERIC;
            }
            break;
        default:
            return VLC_EGENERIC;
    }
    //precompute some values for the C version
    /*const int coefY  = (int)(1.164 * 32768 + 0.5);
    const int coefRV = (int)(1.793 * 32768 + 0.5);
    const int coefGU = (int)(0.213 * 32768 + 0.5);
    const int coefGV = (int)(0.533 * 32768 + 0.5);
    const int coefBU = (int)(2.113 * 32768 + 0.5);
    for (int i=0; i<256; ++i)
    {
        CoefY[i] = coefY * (i-16) + 16384;
        CoefRV[i] = coefRV*(i-128);
        CoefGU[i] = -coefGU*(i-128);
        CoefGV[i] = -coefGV*(i-128);
        CoefBU[i] = coefBU*(i-128);
    }*/
    msg_Dbg(filter, "%4.4s(%dx%d) to %4.4s(%dx%d)",
            (char*)&filter->fmt_in.video.i_chroma, filter->fmt_in.video.i_visible_width, filter->fmt_in.video.i_visible_height,
            (char*)&filter->fmt_out.video.i_chroma, filter->fmt_out.video.i_visible_width, filter->fmt_out.video.i_visible_height);
    return VLC_SUCCESS;
}
