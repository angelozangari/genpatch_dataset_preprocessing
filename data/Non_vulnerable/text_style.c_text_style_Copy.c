}
text_style_t *text_style_Copy( text_style_t *p_dst, const text_style_t *p_src )
{
    if( !p_src )
        return p_dst;
    /* */
    *p_dst = *p_src;
    if( p_src->psz_fontname )
        p_dst->psz_fontname = strdup( p_src->psz_fontname );
    if( p_src->psz_monofontname )
        p_dst->psz_monofontname = strdup( p_src->psz_monofontname );
    return p_dst;
}
