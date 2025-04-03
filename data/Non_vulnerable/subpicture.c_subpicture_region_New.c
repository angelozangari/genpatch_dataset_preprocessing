}
subpicture_region_t *subpicture_region_New( const video_format_t *p_fmt )
{
    subpicture_region_t *p_region = calloc( 1, sizeof(*p_region ) );
    if( !p_region )
        return NULL;
    p_region->fmt = *p_fmt;
    p_region->fmt.p_palette = NULL;
    if( p_fmt->i_chroma == VLC_CODEC_YUVP )
    {
        p_region->fmt.p_palette = calloc( 1, sizeof(*p_region->fmt.p_palette) );
        if( p_fmt->p_palette )
            *p_region->fmt.p_palette = *p_fmt->p_palette;
    }
    p_region->i_alpha = 0xff;
    p_region->p_next = NULL;
    p_region->p_private = NULL;
    p_region->psz_text = NULL;
    p_region->p_style = NULL;
    p_region->p_picture = NULL;
    if( p_fmt->i_chroma == VLC_CODEC_TEXT )
        return p_region;
    p_region->p_picture = picture_NewFromFormat( p_fmt );
    if( !p_region->p_picture )
    {
        free( p_region->fmt.p_palette );
        free( p_region );
        return NULL;
    }
    return p_region;
}
