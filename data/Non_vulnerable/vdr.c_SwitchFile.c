 *****************************************************************************/
static bool SwitchFile( access_t *p_access, unsigned i_file )
{
    access_sys_t *p_sys = p_access->p_sys;
    /* requested file already open? */
    if( p_sys->fd != -1 && p_sys->i_current_file == i_file )
        return true;
    /* close old file */
    if( p_sys->fd != -1 )
    {
        close( p_sys->fd );
        p_sys->fd = -1;
    }
    /* switch */
    if( i_file >= FILE_COUNT )
        return false;
    p_sys->i_current_file = i_file;
    /* open new file */
    char *psz_path = GetFilePath( p_access, i_file );
    if( !psz_path )
        return false;
    p_sys->fd = vlc_open( psz_path, O_RDONLY );
    if( p_sys->fd == -1 )
    {
        msg_Err( p_access, "Failed to open %s: %s", psz_path,
                 vlc_strerror_c(errno) );
        goto error;
    }
    /* cannot handle anything except normal files */
    struct stat st;
    if( fstat( p_sys->fd, &st ) || !S_ISREG( st.st_mode ) )
    {
        msg_Err( p_access, "%s is not a regular file", psz_path );
        goto error;
    }
    OptimizeForRead( p_sys->fd );
    msg_Dbg( p_access, "opened %s", psz_path );
    free( psz_path );
    return true;
error:
    dialog_Fatal (p_access, _("File reading failed"), _("VLC could not"
        " open the file \"%s\" (%s)."), psz_path, vlc_strerror(errno) );
    if( p_sys->fd != -1 )
    {
        close( p_sys->fd );
        p_sys->fd = -1;
    }
    free( psz_path );
    return false;
}
