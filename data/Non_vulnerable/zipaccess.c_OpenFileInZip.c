 *****************************************************************************/
static int OpenFileInZip( access_t *p_access )
{
    access_sys_t *p_sys = p_access->p_sys;
    unzFile file = p_sys->zipFile;
    if( !p_sys->psz_fileInzip )
    {
        return VLC_EGENERIC;
    }
    p_access->info.i_pos = 0;
    unzCloseCurrentFile( file ); /* returns UNZ_PARAMERROR if file not opened */
    if( unzLocateFile( file, p_sys->psz_fileInzip, 0 ) != UNZ_OK )
    {
        msg_Err( p_access, "could not [re]locate file in zip: '%s'",
                 p_sys->psz_fileInzip );
        return VLC_EGENERIC;
    }
    if( unzOpenCurrentFile( file ) != UNZ_OK )
    {
        msg_Err( p_access, "could not [re]open file in zip: '%s'",
                 p_sys->psz_fileInzip );
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
