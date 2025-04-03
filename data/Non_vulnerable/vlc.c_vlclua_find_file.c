}
char *vlclua_find_file( const char *psz_luadirname, const char *psz_name )
{
    char **ppsz_dir_list = NULL;
    vlclua_dir_list( psz_luadirname, &ppsz_dir_list );
    for( char **ppsz_dir = ppsz_dir_list; *ppsz_dir; ppsz_dir++ )
    {
        for( const char **ppsz_ext = ppsz_lua_exts; *ppsz_ext; ppsz_ext++ )
        {
            char *psz_filename;
            struct stat st;
            if( asprintf( &psz_filename, "%s"DIR_SEP"%s%s", *ppsz_dir,
                          psz_name, *ppsz_ext ) < 0 )
            {
                vlclua_dir_list_free( ppsz_dir_list );
                return NULL;
            }
            if( vlc_stat( psz_filename, &st ) == 0
                && S_ISREG( st.st_mode ) )
            {
                vlclua_dir_list_free( ppsz_dir_list );
                return psz_filename;
            }
            free( psz_filename );
        }
    }
    vlclua_dir_list_free( ppsz_dir_list );
    return NULL;
}
