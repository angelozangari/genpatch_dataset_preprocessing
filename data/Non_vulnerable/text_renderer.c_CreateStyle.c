#include "text_renderer.h"
text_style_t *CreateStyle( char *psz_fontname, int i_font_size,
                                  uint32_t i_font_color, uint32_t i_karaoke_bg_color,
                                  int i_style_flags )
{
    text_style_t *p_style = text_style_New();
    if( !p_style )
        return NULL;
    p_style->psz_fontname = psz_fontname ? strdup( psz_fontname ) : NULL;
    p_style->i_font_size  = i_font_size;
    p_style->i_font_color = (i_font_color & 0x00ffffff) >>  0;
    p_style->i_font_alpha = (i_font_color & 0xff000000) >> 24;
    p_style->i_karaoke_background_color = (i_karaoke_bg_color & 0x00ffffff) >>  0;
    p_style->i_karaoke_background_alpha = (i_karaoke_bg_color & 0xff000000) >> 24;
    p_style->i_style_flags |= i_style_flags;
    return p_style;
}
