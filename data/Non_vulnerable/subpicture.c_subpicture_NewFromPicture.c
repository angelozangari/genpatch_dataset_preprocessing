}
subpicture_t *subpicture_NewFromPicture( vlc_object_t *p_obj,
                                         picture_t *p_picture, vlc_fourcc_t i_chroma )
{
    /* */
    video_format_t fmt_in = p_picture->format;
    /* */
    video_format_t fmt_out;
    fmt_out = fmt_in;
    fmt_out.i_chroma = i_chroma;
    /* */
    image_handler_t *p_image = image_HandlerCreate( p_obj );
    if( !p_image )
        return NULL;
    picture_t *p_pip = image_Convert( p_image, p_picture, &fmt_in, &fmt_out );
    image_HandlerDelete( p_image );
    if( !p_pip )
        return NULL;
    subpicture_t *p_subpic = subpicture_New( NULL );
    if( !p_subpic )
    {
         picture_Release( p_pip );
         return NULL;
    }
    p_subpic->i_original_picture_width  = fmt_out.i_width;
    p_subpic->i_original_picture_height = fmt_out.i_height;
    fmt_out.i_sar_num =
    fmt_out.i_sar_den = 0;
    p_subpic->p_region = subpicture_region_New( &fmt_out );
    if( p_subpic->p_region )
    {
        picture_Release( p_subpic->p_region->p_picture );
        p_subpic->p_region->p_picture = p_pip;
    }
    else
    {
        picture_Release( p_pip );
    }
    return p_subpic;
}
