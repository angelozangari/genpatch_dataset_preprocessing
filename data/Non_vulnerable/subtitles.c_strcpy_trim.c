};
static void strcpy_trim( char *d, const char *s )
{
    unsigned char c;
    /* skip leading whitespace */
    while( ((c = *s) != '\0') && !isalnum(c) )
    {
        s++;
    }
    for(;;)
    {
        /* copy word */
        while( ((c = *s) != '\0') && isalnum(c) )
        {
            *d = tolower(c);
            s++; d++;
        }
        if( *s == 0 ) break;
        /* trim excess whitespace */
        while( ((c = *s) != '\0') && !isalnum(c) )
        {
            s++;
        }
        if( *s == 0 ) break;
        *d++ = ' ';
    }
    *d = 0;
}
