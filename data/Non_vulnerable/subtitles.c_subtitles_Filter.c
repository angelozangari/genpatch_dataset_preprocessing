 */
int subtitles_Filter( const char *psz_dir_content )
{
    const char *tmp = strrchr( psz_dir_content, '.');
    if( !tmp )
        return 0;
    tmp++;
    for( int i = 0; sub_exts[i][0]; i++ )
        if( strcasecmp( sub_exts[i], tmp ) == 0 )
            return 1;
    return 0;
}
