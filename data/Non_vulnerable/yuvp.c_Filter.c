 ****************************************************************************/
static picture_t *Filter( filter_t *p_filter, picture_t *p_pic )
{
    picture_t *p_out;
    if( !p_pic )
        return NULL;
    const video_palette_t *p_yuvp = p_filter->fmt_in.video.p_palette;
    assert( p_yuvp != NULL );
    assert( p_filter->fmt_in.video.i_chroma == VLC_CODEC_YUVP );
    assert( p_filter->fmt_in.video.i_width == p_filter->fmt_out.video.i_width );
    assert( p_filter->fmt_in.video.i_height == p_filter->fmt_out.video.i_height );
    /* Request output picture */
    p_out = filter_NewPicture( p_filter );
    if( !p_out )
    {
        picture_Release( p_pic );
        return NULL;
    }
    if( p_filter->fmt_out.video.i_chroma == VLC_CODEC_YUVA )
    {
        for( unsigned int y = 0; y < p_filter->fmt_in.video.i_height; y++ )
        {
            const uint8_t *p_line = &p_pic->p->p_pixels[y*p_pic->p->i_pitch];
            uint8_t *p_y = &p_out->Y_PIXELS[y*p_out->Y_PITCH];
            uint8_t *p_u = &p_out->U_PIXELS[y*p_out->U_PITCH];
            uint8_t *p_v = &p_out->V_PIXELS[y*p_out->V_PITCH];
            uint8_t *p_a = &p_out->A_PIXELS[y*p_out->A_PITCH];
            for( unsigned int x = 0; x < p_filter->fmt_in.video.i_width; x++ )
            {
                const int v = p_line[x];
                if( v > p_yuvp->i_entries )  /* maybe assert ? */
                    continue;
                p_y[x] = p_yuvp->palette[v][0];
                p_u[x] = p_yuvp->palette[v][1];
                p_v[x] = p_yuvp->palette[v][2];
                p_a[x] = p_yuvp->palette[v][3];
            }
        }
    }
    else if( p_filter->fmt_out.video.i_chroma == VLC_CODEC_RGBA )
    {
        video_palette_t rgbp;
        /* Create a RGBA palette */
        rgbp.i_entries = p_yuvp->i_entries;
        for( int i = 0; i < p_yuvp->i_entries; i++ )
        {
            Yuv2Rgb( &rgbp.palette[i][0], &rgbp.palette[i][1], &rgbp.palette[i][2],
                     p_yuvp->palette[i][0], p_yuvp->palette[i][1], p_yuvp->palette[i][2] );
            rgbp.palette[i][3] = p_yuvp->palette[i][3];
        }
        for( unsigned int y = 0; y < p_filter->fmt_in.video.i_height; y++ )
        {
            const uint8_t *p_line = &p_pic->p->p_pixels[y*p_pic->p->i_pitch];
            uint8_t *p_rgba = &p_out->p->p_pixels[y*p_out->p->i_pitch];
            for( unsigned int x = 0; x < p_filter->fmt_in.video.i_width; x++ )
            {
                const int v = p_line[x];
                if( v >= rgbp.i_entries )  /* maybe assert ? */
                    continue;
                p_rgba[4*x+0] = rgbp.palette[v][0];
                p_rgba[4*x+1] = rgbp.palette[v][1];
                p_rgba[4*x+2] = rgbp.palette[v][2];
                p_rgba[4*x+3] = rgbp.palette[v][3];
            }
        }
    }
    else
    {
        video_palette_t rgbp;
        assert( p_filter->fmt_out.video.i_chroma == VLC_CODEC_ARGB );
        /* Create a RGBA palette */
        rgbp.i_entries = p_yuvp->i_entries;
        for( int i = 0; i < p_yuvp->i_entries; i++ )
        {
            Yuv2Rgb( &rgbp.palette[i][1], &rgbp.palette[i][2], &rgbp.palette[i][3],
                     p_yuvp->palette[i][0], p_yuvp->palette[i][1], p_yuvp->palette[i][2] );
            rgbp.palette[i][0] = p_yuvp->palette[i][3];
        }
        for( unsigned int y = 0; y < p_filter->fmt_in.video.i_height; y++ )
        {
            const uint8_t *p_line = &p_pic->p->p_pixels[y*p_pic->p->i_pitch];
            uint8_t *p_rgba = &p_out->p->p_pixels[y*p_out->p->i_pitch];
            for( unsigned int x = 0; x < p_filter->fmt_in.video.i_width; x++ )
            {
                const int v = p_line[x];
                if( v >= rgbp.i_entries )  /* maybe assert ? */
                    continue;
                p_rgba[4*x+0] = rgbp.palette[v][0];
                p_rgba[4*x+1] = rgbp.palette[v][1];
                p_rgba[4*x+2] = rgbp.palette[v][2];
                p_rgba[4*x+3] = rgbp.palette[v][3];
            }
        }
    }
    picture_CopyProperties( p_out, p_pic );
    picture_Release( p_pic );
    return p_out;
}
