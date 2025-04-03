 *****************************************************************************/
int vlclua_scripts_batch_execute( vlc_object_t *p_this,
                                  const char * luadirname,
                                  int (*func)(vlc_object_t *, const char *, const luabatch_context_t *),
                                  void * user_data)
{
    char **ppsz_dir_list = NULL;
    int i_ret;
    if( (i_ret = vlclua_dir_list( luadirname, &ppsz_dir_list )) != VLC_SUCCESS)
        return i_ret;
    i_ret = VLC_EGENERIC;
    for( char **ppsz_dir = ppsz_dir_list; *ppsz_dir; ppsz_dir++ )
    {
        char **ppsz_filelist;
        msg_Dbg( p_this, "Trying Lua scripts in %s", *ppsz_dir );
        int i_files = vlc_scandir( *ppsz_dir, &ppsz_filelist, file_select,
                                   file_compare );
        if( i_files < 0 )
            continue;
        char **ppsz_file = ppsz_filelist;
        char **ppsz_fileend = ppsz_filelist + i_files;
        while( ppsz_file < ppsz_fileend )
        {
            char *psz_filename;
            if( asprintf( &psz_filename,
                          "%s" DIR_SEP "%s", *ppsz_dir, *ppsz_file ) == -1 )
                psz_filename = NULL;
            free( *(ppsz_file++) );
            if( likely(psz_filename != NULL) )
            {
                msg_Dbg( p_this, "Trying Lua playlist script %s",
                         psz_filename );
                i_ret = func( p_this, psz_filename, user_data );
                free( psz_filename );
                if( i_ret == VLC_SUCCESS )
                    break;
            }
        }
        while( ppsz_file < ppsz_fileend )
            free( *(ppsz_file++) );
        free( ppsz_filelist );
        if( i_ret == VLC_SUCCESS )
            break;
    }
    vlclua_dir_list_free( ppsz_dir_list );
    return i_ret;
}
