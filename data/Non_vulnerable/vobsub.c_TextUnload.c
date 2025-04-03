}
static void TextUnload( text_t *txt )
{
    for( int i = 0; i < txt->i_line_count; i++ )
        free( txt->line[i] );
    free( txt->line );
    txt->i_line       = 0;
    txt->i_line_count = 0;
}
