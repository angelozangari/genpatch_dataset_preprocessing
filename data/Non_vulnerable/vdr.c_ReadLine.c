 *****************************************************************************/
static bool ReadLine( char **ppsz_line, size_t *pi_size, FILE *p_file )
{
    ssize_t read = getline( ppsz_line, pi_size, p_file );
    if( read == -1 )
    {
        /* automatically free buffer on eof */
        free( *ppsz_line );
        *ppsz_line = NULL;
        return false;
    }
    if( read > 0 && (*ppsz_line)[ read - 1 ] == '\n' )
        (*ppsz_line)[ read - 1 ] = '\0';
    EnsureUTF8( *ppsz_line );
    return true;
}
