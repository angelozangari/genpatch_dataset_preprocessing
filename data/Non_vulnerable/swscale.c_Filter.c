 ****************************************************************************/
static picture_t *Filter( filter_t *p_filter, picture_t *p_pic )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    const video_format_t *p_fmti = &p_filter->fmt_in.video;
    const video_format_t *p_fmto = &p_filter->fmt_out.video;
    picture_t *p_pic_dst;
    /* Check if format properties changed */
    if( Init( p_filter ) )
    {
        picture_Release( p_pic );
        return NULL;
    }
    /* Request output picture */
    p_pic_dst = filter_NewPicture( p_filter );
    if( !p_pic_dst )
    {
        picture_Release( p_pic );
        return NULL;
    }
    /* */
    picture_t *p_src = p_pic;
    picture_t *p_dst = p_pic_dst;
    if( p_sys->i_extend_factor != 1 )
    {
        p_src = p_sys->p_src_e;
        p_dst = p_sys->p_dst_e;
        CopyPad( p_src, p_pic );
    }
    if( p_sys->b_copy && p_sys->b_swap_uvi == p_sys->b_swap_uvo )
        picture_CopyPixels( p_dst, p_src );
    else if( p_sys->b_copy )
        SwapUV( p_dst, p_src );
    else
        Convert( p_filter, p_sys->ctx, p_dst, p_src, p_fmti->i_visible_height, 0, 3,
                 p_sys->b_swap_uvi, p_sys->b_swap_uvo );
    if( p_sys->ctxA )
    {
        /* We extract the A plane to rescale it, and then we reinject it. */
        if( p_fmti->i_chroma == VLC_CODEC_RGBA || p_fmti->i_chroma == VLC_CODEC_BGRA )
            ExtractA( p_sys->p_src_a, p_src, p_fmti->i_visible_width * p_sys->i_extend_factor, p_fmti->i_visible_height, OFFSET_A );
        else if( p_fmti->i_chroma == VLC_CODEC_ARGB )
            ExtractA( p_sys->p_src_a, p_src, p_fmti->i_visible_width * p_sys->i_extend_factor, p_fmti->i_visible_height, 0 );
        else
            plane_CopyPixels( p_sys->p_src_a->p, p_src->p+A_PLANE );
        Convert( p_filter, p_sys->ctxA, p_sys->p_dst_a, p_sys->p_src_a, p_fmti->i_visible_height, 0, 1, false, false );
        if( p_fmto->i_chroma == VLC_CODEC_RGBA || p_fmto->i_chroma == VLC_CODEC_BGRA )
            InjectA( p_dst, p_sys->p_dst_a, p_fmto->i_visible_width * p_sys->i_extend_factor, p_fmto->i_visible_height, OFFSET_A );
        else if( p_fmto->i_chroma == VLC_CODEC_ARGB )
            InjectA( p_dst, p_sys->p_dst_a, p_fmto->i_visible_width * p_sys->i_extend_factor, p_fmto->i_visible_height, 0 );
        else
            plane_CopyPixels( p_dst->p+A_PLANE, p_sys->p_dst_a->p );
    }
    else if( p_sys->b_add_a )
    {
        /* We inject a complete opaque alpha plane */
        if( p_fmto->i_chroma == VLC_CODEC_RGBA || p_fmto->i_chroma == VLC_CODEC_BGRA )
            FillA( &p_dst->p[0], OFFSET_A );
        else if( p_fmto->i_chroma == VLC_CODEC_ARGB )
            FillA( &p_dst->p[0], 0 );
        else
            FillA( &p_dst->p[A_PLANE], 0 );
    }
    if( p_sys->i_extend_factor != 1 )
    {
        picture_CopyPixels( p_pic_dst, p_dst );
    }
    picture_CopyProperties( p_pic_dst, p_pic );
    picture_Release( p_pic );
    return p_pic_dst;
}
