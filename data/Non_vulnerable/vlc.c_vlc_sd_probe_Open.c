}
static int vlc_sd_probe_Open( vlc_object_t *obj )
{
    vlc_probe_t *probe = (vlc_probe_t *)obj;
    char **ppsz_filelist = NULL;
    char **ppsz_fileend  = NULL;
    char **ppsz_file;
    char *psz_name;
    char **ppsz_dir_list = NULL;
    char **ppsz_dir;
    lua_State *L = NULL;
    vlclua_dir_list( "sd", &ppsz_dir_list );
    for( ppsz_dir = ppsz_dir_list; *ppsz_dir; ppsz_dir++ )
    {
        int i_files;
        if( ppsz_filelist )
        {
            for( ppsz_file = ppsz_filelist; ppsz_file < ppsz_fileend;
                 ppsz_file++ )
                free( *ppsz_file );
            free( ppsz_filelist );
            ppsz_filelist = NULL;
        }
        i_files = vlc_scandir( *ppsz_dir, &ppsz_filelist, file_select,
                                file_compare );
        if( i_files < 1 ) continue;
        ppsz_fileend = ppsz_filelist + i_files;
        for( ppsz_file = ppsz_filelist; ppsz_file < ppsz_fileend; ppsz_file++ )
        {
            char  *psz_filename;
            if( asprintf( &psz_filename,
                          "%s" DIR_SEP "%s", *ppsz_dir, *ppsz_file ) < 0 )
            {
                goto error;
            }
            L = luaL_newstate();
            if( !L )
            {
                msg_Err( probe, "Could not create new Lua State" );
                free( psz_filename );
                goto error;
            }
            luaL_openlibs( L );
            if( vlclua_add_modules_path( L, psz_filename ) )
            {
                msg_Err( probe, "Error while setting the module search path for %s",
                          psz_filename );
                free( psz_filename );
                goto error;
            }
            if( vlclua_dofile( VLC_OBJECT(probe), L, psz_filename ) )
            {
                msg_Err( probe, "Error loading script %s: %s", psz_filename,
                          lua_tostring( L, lua_gettop( L ) ) );
                lua_pop( L, 1 );
                free( psz_filename );
                lua_close( L );
                continue;
            }
            char *psz_longname;
            char *temp = strchr( *ppsz_file, '.' );
            if( temp )
                *temp = '\0';
            lua_getglobal( L, "descriptor" );
            if( !lua_isfunction( L, lua_gettop( L ) ) || lua_pcall( L, 0, 1, 0 ) )
            {
                msg_Warn( probe, "No 'descriptor' function in '%s'", psz_filename );
                lua_pop( L, 1 );
                if( !( psz_longname = strdup( *ppsz_file ) ) )
                {
                    free( psz_filename );
                    goto error;
                }
            }
            else
            {
                lua_getfield( L, -1, "title" );
                if( !lua_isstring( L, -1 ) ||
                    !( psz_longname = strdup( lua_tostring( L, -1 ) ) ) )
                {
                    free( psz_filename );
                    goto error;
                }
            }
            char *psz_file_esc = config_StringEscape( *ppsz_file );
            char *psz_longname_esc = config_StringEscape( psz_longname );
            if( asprintf( &psz_name, "lua{sd='%s',longname='%s'}",
                          psz_file_esc, psz_longname_esc ) < 0 )
            {
                free( psz_file_esc );
                free( psz_longname_esc );
                free( psz_filename );
                free( psz_longname );
                goto error;
            }
            free( psz_file_esc );
            free( psz_longname_esc );
            vlc_sd_probe_Add( probe, psz_name, psz_longname, SD_CAT_INTERNET );
            free( psz_name );
            free( psz_longname );
            free( psz_filename );
            lua_close( L );
        }
    }
    if( ppsz_filelist )
    {
        for( ppsz_file = ppsz_filelist; ppsz_file < ppsz_fileend;
             ppsz_file++ )
            free( *ppsz_file );
        free( ppsz_filelist );
    }
    vlclua_dir_list_free( ppsz_dir_list );
    return VLC_PROBE_CONTINUE;
error:
    if( ppsz_filelist )
    {
        for( ppsz_file = ppsz_filelist; ppsz_file < ppsz_fileend;
             ppsz_file++ )
            free( *ppsz_file );
        free( ppsz_filelist );
    }
    if( L )
        lua_close( L );
    vlclua_dir_list_free( ppsz_dir_list );
    return VLC_ENOMEM;
}
