 *****************************************************************************/
int AccessOpen( vlc_object_t *p_this )
{
    access_t     *p_access = (access_t*)p_this;
    access_sys_t *p_sys;
    int i_ret              = VLC_EGENERIC;
    char *psz_pathToZip = NULL, *psz_path = NULL, *psz_sep = NULL;
    if( !strstr( p_access->psz_location, ZIP_SEP ) )
    {
        msg_Dbg( p_access, "location does not contain separator " ZIP_SEP );
        return VLC_EGENERIC;
    }
    p_access->p_sys = p_sys = (access_sys_t*)
            calloc( 1, sizeof( access_sys_t ) );
    if( unlikely( !p_sys ) )
        return VLC_ENOMEM;
    /* Split the MRL */
    psz_path = xstrdup( p_access->psz_location );
    psz_sep = strstr( psz_path, ZIP_SEP );
    *psz_sep = '\0';
    psz_pathToZip = unescapeXml( psz_path );
    if( !psz_pathToZip )
    {
        /* Maybe this was not an encoded string */
        msg_Dbg( p_access, "not an encoded URL  Trying file '%s'",
                 psz_path );
        psz_pathToZip = strdup( psz_path );
        if( unlikely( !psz_pathToZip ) )
        {
            i_ret = VLC_ENOMEM;
            goto exit;
        }
    }
    p_sys->psz_fileInzip = unescapeXml( psz_sep + ZIP_SEP_LEN );
    if( unlikely( !p_sys->psz_fileInzip ) )
    {
        p_sys->psz_fileInzip = strdup( psz_sep + ZIP_SEP_LEN );
        if( unlikely( !p_sys->psz_fileInzip ) )
        {
            i_ret = VLC_ENOMEM;
            goto exit;
        }
    }
    /* Define IO functions */
    zlib_filefunc_def func;
    func.zopen_file   = ZipIO_Open;
    func.zread_file   = ZipIO_Read;
    func.zwrite_file  = ZipIO_Write; // see comment
    func.ztell_file   = ZipIO_Tell;
    func.zseek_file   = ZipIO_Seek;
    func.zclose_file  = ZipIO_Close;
    func.zerror_file  = ZipIO_Error;
    func.opaque       = p_access;
    /* Open zip archive */
    p_access->p_sys->zipFile = unzOpen2( psz_pathToZip, &func );
    if( !p_access->p_sys->zipFile )
    {
        msg_Err( p_access, "not a valid zip archive: '%s'", psz_pathToZip );
        i_ret = VLC_EGENERIC;
        goto exit;
    }
    /* Open file in zip */
    if( ( i_ret = OpenFileInZip( p_access ) ) != VLC_SUCCESS )
        goto exit;
    /* Set callback */
    ACCESS_SET_CALLBACKS( AccessRead, NULL, AccessControl, AccessSeek );
    p_access->info.i_pos  = 0;
    p_access->info.b_eof  = false;
    i_ret = VLC_SUCCESS;
exit:
    if( i_ret != VLC_SUCCESS )
    {
        if( p_access->p_sys->zipFile )
        {
            unzCloseCurrentFile( p_access->p_sys->zipFile );
            unzClose( p_access->p_sys->zipFile );
        }
        free( p_sys->psz_fileInzip );
        free( p_sys->fileFunctions );
        free( p_sys );
    }
    free( psz_pathToZip );
    free( psz_path );
    return i_ret;
}
