}
static int OpaquePage( picture_t *p_src, const vbi_page *p_page,
                       const video_format_t fmt, bool b_opaque, const int text_offset )
{
    unsigned int    x, y;
    assert( fmt.i_chroma == VLC_CODEC_RGBA );
    /* Kludge since zvbi doesn't provide an option to specify opacity. */
    for( y = 0; y < fmt.i_height; y++ )
    {
        for( x = 0; x < fmt.i_width; x++ )
        {
            const vbi_opacity opacity = p_page->text[ text_offset + y/10 * p_page->columns + x/12 ].opacity;
            const int background = p_page->text[ text_offset + y/10 * p_page->columns + x/12 ].background;
            uint32_t *p_pixel = (uint32_t*)&p_src->p->p_pixels[y * p_src->p->i_pitch + 4*x];
            switch( opacity )
            {
            /* Show video instead of this character */
            case VBI_TRANSPARENT_SPACE:
                *p_pixel = 0;
                break;
            /* Display foreground and background color */
            /* To make the boxed text "closed captioning" transparent
             * change true to false.
             */
            case VBI_OPAQUE:
            /* alpha blend video into background color */
            case VBI_SEMI_TRANSPARENT:
                if( b_opaque )
                    break;
            /* Full text transparency. only foreground color is show */
            case VBI_TRANSPARENT_FULL:
                if( (*p_pixel) == (0xff000000 | p_page->color_map[background] ) )
                    *p_pixel = 0;
                break;
            }
        }
    }
    /* end of kludge */
    return VLC_SUCCESS;
}
