}
static char *TextGetLine( text_t *txt )
{
    if( txt->i_line >= txt->i_line_count )
        return( NULL );
    return txt->line[txt->i_line++];
}
