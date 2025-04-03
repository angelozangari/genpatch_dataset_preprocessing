}
int HandleTT(font_stack_t **p_fonts, const char *p_fontfamily )
{
    char      *psz_unused_fontname = NULL;
    uint32_t   i_font_color = 0xffffff;
    uint32_t   i_karaoke_bg_color = 0x00ffffff;
    int        i_font_size  = STYLE_DEFAULT_FONT_SIZE;
    /* Default all attributes to the top font in the stack -- in case not
     * all attributes are specified in the sub-font
     */
    PeekFont( p_fonts,
             &psz_unused_fontname,
             &i_font_size,
             &i_font_color,
             &i_karaoke_bg_color );
    /* Keep all the parent's font attributes, but change to a monospace font */
    return PushFont( p_fonts,
                   p_fontfamily,
                   i_font_size,
                   i_font_color,
                   i_karaoke_bg_color );
}
