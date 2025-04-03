 *****************************************************************************/
static const char *BaseName( const char *psz_path )
{
    const char *psz_name = psz_path + strlen( psz_path );
    /* skip superfluous separators at the end */
    while( psz_name > psz_path && psz_name[-1] == DIR_SEP_CHAR )
        --psz_name;
    /* skip last component */
    while( psz_name > psz_path && psz_name[-1] != DIR_SEP_CHAR )
        --psz_name;
    return psz_name;
}
