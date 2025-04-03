}
int PeekFont( font_stack_t **p_font, char **psz_name, int *i_size,
                     uint32_t *i_color, uint32_t *i_karaoke_bg_color )
{
    font_stack_t *p_last;
    if( !p_font || !*p_font )
        return VLC_EGENERIC;
    for( p_last=*p_font;
         p_last->p_next;
         p_last=p_last->p_next )
    ;
    *psz_name            = p_last->psz_name;
    *i_size              = p_last->i_size;
    *i_color             = p_last->i_color;
    *i_karaoke_bg_color  = p_last->i_karaoke_bg_color;
    return VLC_SUCCESS;
}
