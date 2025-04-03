}
subpicture_region_private_t *subpicture_region_private_New( video_format_t *p_fmt )
{
    subpicture_region_private_t *p_private = malloc( sizeof(*p_private) );
    if( !p_private )
        return NULL;
    p_private->fmt = *p_fmt;
    if( p_fmt->p_palette )
    {
        p_private->fmt.p_palette = malloc( sizeof(*p_private->fmt.p_palette) );
        if( p_private->fmt.p_palette )
            *p_private->fmt.p_palette = *p_fmt->p_palette;
    }
    p_private->p_picture = NULL;
    return p_private;
}
