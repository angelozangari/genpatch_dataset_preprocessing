 */
void filename_sanitize( char *str )
{
    unsigned char c;
    /* Special file names, not allowed */
    if( !strcmp( str, "." ) || !strcmp( str, ".." ) )
    {
        while( *str )
            *(str++) = '_';
        return;
    }
    /* On platforms not using UTF-7, VLC cannot access non-Unicode paths.
     * Also, some file systems require Unicode file names.
     * NOTE: This may inserts '?' thus is done replacing '?' with '_'. */
    EnsureUTF8( str );
    /* Avoid leading spaces to please Windows. */
    while( (c = *str) != '\0' )
    {
        if( c != ' ' )
            break;
        *(str++) = '_';
    }
    char *start = str;
    while( (c = *str) != '\0' )
    {
        /* Non-printable characters are not a good idea */
        if( c < 32 )
            *str = '_';
        /* This is the list of characters not allowed by Microsoft.
         * We also black-list them on Unix as they may be confusing, and are
         * not supported by some file system types (notably CIFS). */
        else if( strchr( "/:\\*\"?|<>", c ) != NULL )
            *str = '_';
        str++;
    }
    /* Avoid trailing spaces also to please Windows. */
    while( str > start )
    {
        if( *(--str) != ' ' )
            break;
        *str = '_';
    }
}
