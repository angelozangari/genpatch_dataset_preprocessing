 *****************************************************************************/
static ssize_t Read( access_t *p_access, uint8_t *p_buffer, size_t i_len )
{
    access_sys_t *p_sys = p_access->p_sys;
    if( p_sys->fd == -1 )
    {
        /* no more data */
        p_access->info.b_eof = true;
        return 0;
    }
    ssize_t i_ret = read( p_sys->fd, p_buffer, i_len );
    if( i_ret > 0 )
    {
        /* success */
        p_access->info.i_pos += i_ret;
        UpdateFileSize( p_access );
        FindSeekpoint( p_access );
        return i_ret;
    }
    else if( i_ret == 0 )
    {
        /* check for new files in case the recording is still active */
        if( p_sys->i_current_file >= FILE_COUNT - 1 )
            ImportNextFile( p_access );
        /* play next file */
        SwitchFile( p_access, p_sys->i_current_file + 1 );
        return -1;
    }
    else if( errno == EINTR )
    {
        /* try again later */
        return -1;
    }
    else
    {
        /* abort on read error */
        msg_Err( p_access, "failed to read (%s)", vlc_strerror_c(errno) );
        dialog_Fatal( p_access, _("File reading failed"),
                      _("VLC could not read the file (%s)."),
                      vlc_strerror(errno) );
        SwitchFile( p_access, -1 );
        return 0;
    }
}
