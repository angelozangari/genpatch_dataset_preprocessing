}
static int RenderText( filter_t *p_filter, subpicture_region_t *p_region_out,
                       subpicture_region_t *p_region_in,
                       const vlc_fourcc_t *p_chroma_list )
{
    VLC_UNUSED(p_chroma_list);
    filter_sys_t *p_sys = p_filter->p_sys;
    svg_rendition_t *p_svg = NULL;
    char *psz_string;
    /* Sanity check */
    if( !p_region_in || !p_region_out ) return VLC_EGENERIC;
    psz_string = p_region_in->psz_text;
    if( !psz_string || !*psz_string ) return VLC_EGENERIC;
    p_svg = malloc( sizeof( svg_rendition_t ) );
    if( !p_svg )
        return VLC_ENOMEM;
    p_region_out->i_x = p_region_in->i_x;
    p_region_out->i_y = p_region_in->i_y;
    /* Check if the data is SVG or pure text. In the latter case,
       convert the text to SVG. FIXME: find a better test */
    if( strstr( psz_string, "<svg" ))
    {
        /* Data is SVG: duplicate */
        p_svg->psz_text = strdup( psz_string );
        if( !p_svg->psz_text )
        {
            free( p_svg );
            return VLC_ENOMEM;
        }
    }
    else
    {
        /* Data is text. Convert to SVG */
        /* FIXME: handle p_style attributes */
        int length;
        char* psz_template = p_sys->psz_template;
        length = strlen( psz_string ) + strlen( psz_template ) + 42;
        p_svg->psz_text = calloc( 1, length + 1 );
        if( !p_svg->psz_text )
        {
            free( p_svg );
            return VLC_ENOMEM;
        }
        snprintf( p_svg->psz_text, length, psz_template, psz_string );
    }
    p_svg->i_width = p_sys->i_width;
    p_svg->i_height = p_sys->i_height;
    p_svg->i_chroma = VLC_CODEC_YUVA;
    /* Render the SVG.
       The input data is stored in the p_string structure,
       and the function updates the p_rendition attribute. */
    svg_RenderPicture( p_filter, p_svg );
    Render( p_filter, p_region_out, p_svg, p_svg->i_width, p_svg->i_height );
    FreeString( p_svg );
    return VLC_SUCCESS;
}
