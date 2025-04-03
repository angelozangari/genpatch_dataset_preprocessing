 *****************************************************************************/
static FILE *OpenRelativeFile( access_t *p_access, const char *psz_file )
{
    /* build path and add extension */
    char *psz_path;
    if( asprintf( &psz_path, "%s" DIR_SEP "%s%s",
        p_access->psz_filepath, psz_file,
        p_access->p_sys->b_ts_format ? "" : ".vdr" ) == -1 )
        return NULL;
    FILE *file = vlc_fopen( psz_path, "rb" );
    if( !file )
        msg_Warn( p_access, "Failed to open %s: %s", psz_path,
                  vlc_strerror_c(errno) );
    free( psz_path );
    return file;
}
