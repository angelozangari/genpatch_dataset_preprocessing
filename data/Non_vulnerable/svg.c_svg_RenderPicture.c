}
static void svg_RenderPicture( filter_t *p_filter,
                               svg_rendition_t *p_svg )
{
    /* Render the SVG string p_string->psz_text into a new picture_t
       p_string->p_rendition with dimensions ( ->i_width, ->i_height ) */
    RsvgHandle *p_handle;
    GError *error = NULL;
    p_svg->p_rendition = NULL;
    p_handle = rsvg_handle_new();
    if( !p_handle )
    {
        msg_Err( p_filter, "Error creating SVG reader" );
        return;
    }
    rsvg_handle_set_size_callback( p_handle, svg_SizeCallback, p_filter, NULL );
    if( ! rsvg_handle_write( p_handle,
                 ( guchar* )p_svg->psz_text, strlen( p_svg->psz_text ),
                 &error ) )
    {
        msg_Err( p_filter, "error while rendering SVG: %s", error->message );
        g_object_unref( G_OBJECT( p_handle ) );
        return;
    }
    if( ! rsvg_handle_close( p_handle, &error ) )
    {
        msg_Err( p_filter, "error while rendering SVG (close): %s", error->message );
        g_object_unref( G_OBJECT( p_handle ) );
        return;
    }
    p_svg->p_rendition = rsvg_handle_get_pixbuf( p_handle );
    g_object_unref( G_OBJECT( p_handle ) );
}
