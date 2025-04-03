 */
void path_sanitize( char *str )
{
#if defined( _WIN32 ) || defined( __OS2__ )
    /* check drive prefix if path is absolute */
    if( (((unsigned char)(str[0] - 'A') < 26)
      || ((unsigned char)(str[0] - 'a') < 26)) && (':' == str[1]) )
        str += 2;
#endif
    while( *str )
    {
#if defined( __APPLE__ )
        if( *str == ':' )
            *str = '_';
#elif defined( _WIN32 ) || defined( __OS2__ )
        if( strchr( "*\"?:|<>", *str ) )
            *str = '_';
        if( *str == '/' )
            *str = DIR_SEP_CHAR;
#endif
        str++;
    }
}
