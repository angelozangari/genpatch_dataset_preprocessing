}
static int get_last_visible_row( vbi_char *p_text, int rows, int columns)
{
    for ( int i = rows * columns - 1; i >= 0; i-- )
    {
        if (p_text[i].opacity != VBI_TRANSPARENT_SPACE)
        {
            return i / columns;
        }
    }
    return -1;
}
