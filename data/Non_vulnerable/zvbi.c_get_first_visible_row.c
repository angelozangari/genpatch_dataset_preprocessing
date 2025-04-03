}
static int get_first_visible_row( vbi_char *p_text, int rows, int columns)
{
    for ( int i = 0; i < rows * columns; i++ )
    {
        if ( p_text[i].opacity != VBI_TRANSPARENT_SPACE )
        {
            return i / columns;
        }
    }
    return -1;
}
