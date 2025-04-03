}
int PopFont( font_stack_t **p_font )
{
    font_stack_t *p_last, *p_next_to_last;
    if( !p_font || !*p_font )
        return VLC_EGENERIC;
    p_next_to_last = NULL;
    for( p_last = *p_font;
         p_last->p_next;
         p_last = p_last->p_next )
    {
        p_next_to_last = p_last;
    }
    if( p_next_to_last )
        p_next_to_last->p_next = NULL;
    else
        *p_font = NULL;
    free( p_last->psz_name );
    free( p_last );
    return VLC_SUCCESS;
}
