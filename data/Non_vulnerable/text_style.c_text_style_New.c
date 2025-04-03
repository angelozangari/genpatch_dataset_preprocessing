/* */
text_style_t *text_style_New( void )
{
    text_style_t *p_style = calloc( 1, sizeof(*p_style) );
    if( !p_style )
        return NULL;
    /* initialize to default text style */
    p_style->psz_fontname = NULL;
    p_style->psz_monofontname = NULL;
    p_style->i_font_size = STYLE_DEFAULT_FONT_SIZE;
    p_style->i_font_color = 0xffffff;
    p_style->i_font_alpha = 0xff;
    p_style->i_style_flags = STYLE_OUTLINE;
    p_style->i_outline_color = 0x000000;
    p_style->i_outline_alpha = 0xff;
    p_style->i_shadow_color = 0x000000;
    p_style->i_shadow_alpha = 0xff;
    p_style->i_background_color = 0xffffff;
    p_style->i_background_alpha = 0x80;
    p_style->i_karaoke_background_color = 0xffffff;
    p_style->i_karaoke_background_alpha = 0xff;
    p_style->i_outline_width = 1;
    p_style->i_shadow_width = 0;
    p_style->i_spacing = -1;
    return p_style;
}
