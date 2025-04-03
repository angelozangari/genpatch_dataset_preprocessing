}
static size_t str8len( const char *psz_string )
{
    const char *psz_tmp = psz_string;
    size_t i=0;
    while ( *psz_tmp )
    {
        if ( (*psz_tmp & 0xC0) != 0x80 ) i++;
        psz_tmp++;
    }
    return i;
}
