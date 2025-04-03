}
static int TextLoad( text_t *txt, stream_t *s )
{
    char **lines = NULL;
    size_t n = 0;
    /* load the complete file */
    for( ;; )
    {
        char *psz = stream_ReadLine( s );
        char **ppsz_new;
        if( psz == NULL || (n >= INT_MAX/sizeof(char *)) )
        {
            free( psz );
            break;
        }
        ppsz_new = realloc( lines, (n + 1) * sizeof (char *) );
        if( ppsz_new == NULL )
        {
            free( psz );
            break;
        }
        lines = ppsz_new;
        lines[n++] = psz;
    }
    txt->i_line_count = n;
    txt->i_line       = 0;
    txt->line         = lines;
    return VLC_SUCCESS;
}
