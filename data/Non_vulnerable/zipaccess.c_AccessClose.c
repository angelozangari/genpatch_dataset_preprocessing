 *****************************************************************************/
void AccessClose( vlc_object_t *p_this )
{
    access_t     *p_access = (access_t*)p_this;
    access_sys_t *p_sys = p_access->p_sys;
    if( p_sys )
    {
        unzFile file = p_sys->zipFile;
        if( file )
        {
            unzCloseCurrentFile( file );
            unzClose( file );
        }
        free( p_sys->psz_fileInzip );
        free( p_sys->fileFunctions );
        free( p_sys );
    }
}
