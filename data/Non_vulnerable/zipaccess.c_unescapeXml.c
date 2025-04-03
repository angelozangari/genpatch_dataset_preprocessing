 *****************************************************************************/
static char *unescapeXml( const char *psz_text )
{
    char *psz_ret = malloc( strlen( psz_text ) + 1 );
    if( unlikely( !psz_ret ) ) return NULL;
    char *psz_tmp = psz_ret;
    for( char *psz_iter = (char*) psz_text; *psz_iter; ++psz_iter, ++psz_tmp )
    {
        if( *psz_iter == '?' )
        {
            int i_value;
            if( !sscanf( ++psz_iter, "%02x", &i_value ) )
            {
                /* Invalid number: URL incorrectly encoded */
                free( psz_ret );
                return NULL;
            }
            *psz_tmp = (char) i_value;
            psz_iter++;
        }
        else if( isAllowedChar( *psz_iter ) )
        {
            *psz_tmp = *psz_iter;
        }
        else
        {
            /* Invalid character encoding for the URL */
            free( psz_ret );
            return NULL;
        }
    }
    *psz_tmp = '\0';
    return psz_ret;
}
