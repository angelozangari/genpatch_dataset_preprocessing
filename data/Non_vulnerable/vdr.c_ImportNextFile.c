 *****************************************************************************/
static bool ImportNextFile( access_t *p_access )
{
    access_sys_t *p_sys = p_access->p_sys;
    char *psz_path = GetFilePath( p_access, FILE_COUNT );
    if( !psz_path )
        return false;
    struct stat st;
    if( vlc_stat( psz_path, &st ) )
    {
        msg_Dbg( p_access, "could not stat %s: %s", psz_path,
                 vlc_strerror_c(errno) );
        free( psz_path );
        return false;
    }
    if( !S_ISREG( st.st_mode ) )
    {
        msg_Dbg( p_access, "%s is not a regular file", psz_path );
        free( psz_path );
        return false;
    }
    msg_Dbg( p_access, "%s exists", psz_path );
    free( psz_path );
    ARRAY_APPEND( p_sys->file_sizes, st.st_size );
    p_sys->size += st.st_size;
    return true;
}
