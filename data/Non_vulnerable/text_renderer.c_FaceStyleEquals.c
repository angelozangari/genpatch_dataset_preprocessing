}
bool FaceStyleEquals( const text_style_t *p_style1,
                             const text_style_t *p_style2 )
{
    if( !p_style1 || !p_style2 )
        return false;
    if( p_style1 == p_style2 )
        return true;
    const int i_style_mask = STYLE_BOLD | STYLE_ITALIC;
    return (p_style1->i_style_flags & i_style_mask) == (p_style2->i_style_flags & i_style_mask) &&
           !strcmp( p_style1->psz_fontname, p_style2->psz_fontname );
}
