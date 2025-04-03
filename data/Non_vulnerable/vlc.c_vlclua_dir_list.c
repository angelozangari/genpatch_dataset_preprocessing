}
int vlclua_dir_list( const char *luadirname, char ***pppsz_dir_list )
{
#define MAX_DIR_LIST_SIZE 5
    *pppsz_dir_list = malloc(MAX_DIR_LIST_SIZE*sizeof(char *));
    if (!*pppsz_dir_list)
        return VLC_EGENERIC;
    char **ppsz_dir_list = *pppsz_dir_list;
    int i = 0;
    char *datadir = config_GetUserDir( VLC_DATA_DIR );
    if( likely(datadir != NULL)
     && likely(asprintf( &ppsz_dir_list[i], "%s"DIR_SEP"lua"DIR_SEP"%s",
                         datadir, luadirname ) != -1) )
        i++;
    free( datadir );
#if !(defined(__APPLE__) || defined(_WIN32))
    char *psz_libpath = config_GetLibDir();
    if( likely(psz_libpath != NULL) )
    {
        if( likely(asprintf( &ppsz_dir_list[i], "%s"DIR_SEP"lua"DIR_SEP"%s",
                             psz_libpath, luadirname ) != -1) )
            i++;
        free( psz_libpath );
    }
#endif
    char *psz_datapath = config_GetDataDir();
    if( likely(psz_datapath != NULL) )
    {
        if( likely(asprintf( &ppsz_dir_list[i], "%s"DIR_SEP"lua"DIR_SEP"%s",
                              psz_datapath, luadirname ) != -1) )
            i++;
#if defined(__APPLE__)
        if( likely(asprintf( &ppsz_dir_list[i],
                             "%s"DIR_SEP"share"DIR_SEP"lua"DIR_SEP"%s",
                             psz_datapath, luadirname ) != -1) )
            i++;
#endif
        free( psz_datapath );
    }
    ppsz_dir_list[i] = NULL;
    assert( i < MAX_DIR_LIST_SIZE);
    return VLC_SUCCESS;
}
