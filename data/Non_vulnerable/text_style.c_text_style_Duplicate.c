}
text_style_t *text_style_Duplicate( const text_style_t *p_src )
{
    if( !p_src )
        return NULL;
    text_style_t *p_dst = calloc( 1, sizeof(*p_dst) );
    if( p_dst )
        text_style_Copy( p_dst, p_src );
    return p_dst;
}
