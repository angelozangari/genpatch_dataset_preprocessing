}
static void FreeString( svg_rendition_t *p_svg )
{
    free( p_svg->psz_text );
    /* p_svg->p_rendition is a GdkPixbuf, and its allocation is
       managed through ref. counting */
    if( p_svg->p_rendition )
        g_object_unref( p_svg->p_rendition );
    free( p_svg );
}
