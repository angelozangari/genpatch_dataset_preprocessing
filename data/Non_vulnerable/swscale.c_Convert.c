}
static void Convert( filter_t *p_filter, struct SwsContext *ctx,
                     picture_t *p_dst, picture_t *p_src, int i_height, int i_plane_start, int i_plane_count,
                     bool b_swap_uvi, bool b_swap_uvo )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    uint8_t palette[AVPALETTE_SIZE];
    uint8_t *src[4]; int src_stride[4];
    uint8_t *dst[4]; int dst_stride[4];
    GetPixels( src, src_stride, p_sys->desc_in, &p_filter->fmt_in.video,
               p_src, i_plane_count, b_swap_uvi );
    if( p_filter->fmt_in.video.i_chroma == VLC_CODEC_RGBP )
    {
        memset( palette, 0, sizeof(palette) );
        if( p_filter->fmt_in.video.p_palette )
            memcpy( palette, p_filter->fmt_in.video.p_palette->palette,
                    __MIN( sizeof(video_palette_t), AVPALETTE_SIZE ) );
        src[1] = palette;
        src_stride[1] = 4;
    }
    GetPixels( dst, dst_stride, p_sys->desc_out, &p_filter->fmt_out.video,
               p_dst, i_plane_count, b_swap_uvo );
#if LIBSWSCALE_VERSION_INT  >= ((0<<16)+(5<<8)+0)
    sws_scale( ctx, src, src_stride, 0, i_height,
               dst, dst_stride );
#else
    sws_scale_ordered( ctx, src, src_stride, 0, i_height,
                       dst, dst_stride );
#endif
}
