}
text_style_t *GetStyleFromFontStack( filter_t *p_filter,
                                     font_stack_t **p_fonts,
                                     text_style_t *style,
                                     int i_style_flags )
{
    char       *psz_fontname = NULL;
    uint32_t    i_font_color = var_InheritInteger( p_filter, "freetype-color" );
    i_font_color = VLC_CLIP( i_font_color, 0, 0xFFFFFF );
    i_font_color = i_font_color & 0x00ffffff;
    int         i_font_size  = style->i_font_size;
    uint32_t    i_karaoke_bg_color = i_font_color;
    if( PeekFont( p_fonts, &psz_fontname, &i_font_size,
                  &i_font_color, &i_karaoke_bg_color ) )
        return NULL;
    return CreateStyle( psz_fontname, i_font_size, i_font_color,
                        i_karaoke_bg_color,
                        i_style_flags );
}
