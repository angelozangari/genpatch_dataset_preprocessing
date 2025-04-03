 *****************************************************************************/
static int AccessSeek( access_t *p_access, uint64_t seek_len )
{
    access_sys_t *p_sys = p_access->p_sys;
    assert( p_sys );
    unzFile file = p_sys->zipFile;
    if( !file )
    {
        msg_Err( p_access, "archive not opened !" );
        return VLC_EGENERIC;
    }
    /* Reopen file in zip if needed */
    if( p_access->info.i_pos > seek_len )
    {
        OpenFileInZip( p_access );
    }
    /* Read seek_len data and drop it */
    unsigned i_seek = 0;
    int i_read = 1;
    char *p_buffer = ( char* ) calloc( 1, ZIP_BUFFER_LEN );
    if( unlikely( !p_buffer ) )
        return VLC_EGENERIC;
    while( ( i_seek < seek_len ) && ( i_read > 0 ) )
    {
        i_read = ( seek_len - i_seek < ZIP_BUFFER_LEN )
               ? ( seek_len - i_seek ) : ZIP_BUFFER_LEN;
        i_read = unzReadCurrentFile( file, p_buffer, i_read );
        if( i_read < 0 )
        {
            msg_Warn( p_access, "could not seek in file" );
            free( p_buffer );
            return VLC_EGENERIC;
        }
        else
        {
            i_seek += i_read;
        }
    }
    free( p_buffer );
    p_access->info.i_pos = unztell( file );
    return VLC_SUCCESS;
}
