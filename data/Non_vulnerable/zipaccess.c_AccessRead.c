 *****************************************************************************/
static ssize_t AccessRead( access_t *p_access, uint8_t *p_buffer, size_t sz )
{
    access_sys_t *p_sys = p_access->p_sys;
    assert( p_sys );
    unzFile file = p_sys->zipFile;
    if( !file )
    {
        msg_Err( p_access, "archive not opened !" );
        return VLC_EGENERIC;
    }
    int i_read = 0;
    i_read = unzReadCurrentFile( file, p_buffer, sz );
    p_access->info.i_pos = unztell( file );
    return ( i_read >= 0 ? i_read : VLC_EGENERIC );
}
