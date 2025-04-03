}
static int whiteonly( const char *s )
{
    unsigned char c;
    while( (c = *s) != '\0' )
    {
        if( isalnum( c ) )
            return 0;
        s++;
    }
    return 1;
}
