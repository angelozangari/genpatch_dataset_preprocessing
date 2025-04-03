}
static char * str8indup( const char *psz_string, size_t i_skip, size_t n )
{
    while( i_skip && *psz_string )
    {
        if ( (*psz_string & 0xC0) != 0x80 ) i_skip--;
        psz_string++;
    }
    if ( ! *psz_string || i_skip ) return NULL;
    const char *psz_tmp = psz_string;
    while( n && *psz_tmp )
    {
        if ( (*psz_tmp & 0xC0) != 0x80 ) n--;
        psz_tmp++;
    }
    return strndup( psz_string, psz_tmp - psz_string );
}
