 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    access_t *p_access = (access_t*)p_this;
    if( !p_access->psz_filepath )
        return VLC_EGENERIC;
    /* Some tests can be skipped if this module was explicitly requested.
     * That way, the user can play "corrupt" recordings if necessary
     * and we can avoid false positives in the general case. */
    bool b_strict = strcmp( p_access->psz_access, "vdr" );
    /* Do a quick test based on the directory name to see if this
     * directory might contain a VDR recording. We can be reasonably
     * sure if ScanDirectory() actually finds files. */
    if( b_strict )
    {
        char psz_extension[4];
        int i_length = 0;
        const char *psz_name = BaseName( p_access->psz_filepath );
        if( sscanf( psz_name, "%*u-%*u-%*u.%*u.%*u.%*u%*[-.]%*u.%3s%n",
            psz_extension, &i_length ) != 1 || strcasecmp( psz_extension, "rec" ) ||
            ( psz_name[i_length] != DIR_SEP_CHAR && psz_name[i_length] != '\0' ) )
            return VLC_EGENERIC;
    }
    /* Only directories can be recordings */
    struct stat st;
    if( vlc_stat( p_access->psz_filepath, &st ) ||
        !S_ISDIR( st.st_mode ) )
        return VLC_EGENERIC;
    access_sys_t *p_sys;
    STANDARD_READ_ACCESS_INIT;
    p_sys->fd = -1;
    p_sys->cur_seekpoint = 0;
    p_sys->fps = var_InheritFloat( p_access, "vdr-fps" );
    ARRAY_INIT( p_sys->file_sizes );
    /* Import all files and prepare playback. */
    if( !ScanDirectory( p_access ) ||
        !SwitchFile( p_access, 0 ) )
    {
        Close( p_this );
        return VLC_EGENERIC;
    }
    free( p_access->psz_demux );
    p_access->psz_demux = strdup( p_sys->b_ts_format ? "ts" : "ps" );
    return VLC_SUCCESS;
}
