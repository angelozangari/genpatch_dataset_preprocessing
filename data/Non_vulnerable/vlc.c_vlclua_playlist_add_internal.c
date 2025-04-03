#undef vlclua_playlist_add_internal
int vlclua_playlist_add_internal( vlc_object_t *p_this, lua_State *L,
                                    playlist_t *p_playlist,
                                    input_item_t *p_parent, bool b_play )
{
    int i_count = 0;
    input_item_node_t *p_parent_node = NULL;
    assert( p_parent || p_playlist );
    /* playlist */
    if( lua_istable( L, -1 ) )
    {
        if( p_parent ) p_parent_node = input_item_node_Create( p_parent );
        lua_pushnil( L );
        /* playlist nil */
        while( lua_next( L, -2 ) )
        {
            /* playlist key item */
            /* <Parse playlist item> */
            if( lua_istable( L, -1 ) )
            {
                lua_getfield( L, -1, "path" );
                /* playlist key item path */
                if( lua_isstring( L, -1 ) )
                {
                    char         *psz_oldurl   = NULL;
                    const char   *psz_path     = NULL;
                    char         *psz_u8path   = NULL;
                    const char   *psz_name     = NULL;
                    char        **ppsz_options = NULL;
                    int           i_options    = 0;
                    mtime_t       i_duration   = -1;
                    input_item_t *p_input;
                    /* Read path and name */
                    if (p_parent) {
                        psz_oldurl = input_item_GetURI( p_parent );
                        msg_Dbg( p_this, "old path: %s", psz_oldurl );
                    }
                    psz_path = lua_tostring( L, -1 );
                    msg_Dbg( p_this, "Path: %s", psz_path );
                    lua_getfield( L, -2, "name" );
                    /* playlist key item path name */
                    if( lua_isstring( L, -1 ) )
                    {
                        psz_name = lua_tostring( L, -1 );
                        msg_Dbg( p_this, "Name: %s", psz_name );
                    }
                    else
                    {
                        if( !lua_isnil( L, -1 ) )
                            msg_Warn( p_this, "Playlist item name should be a string." );
                        psz_name = NULL;
                    }
                    /* Read duration */
                    lua_getfield( L, -3, "duration" );
                    /* playlist key item path name duration */
                    if( lua_isnumber( L, -1 ) )
                    {
                        i_duration = (mtime_t)(lua_tonumber( L, -1 )*1e6);
                    }
                    else if( !lua_isnil( L, -1 ) )
                    {
                        msg_Warn( p_this, "Playlist item duration should be a number (in seconds)." );
                    }
                    lua_pop( L, 1 ); /* pop "duration" */
                    /* playlist key item path name */
                    /* Read options: item must be on top of stack */
                    lua_pushvalue( L, -3 );
                    /* playlist key item path name item */
                    vlclua_read_options( p_this, L, &i_options, &ppsz_options );
                    /* Create input item */
                    p_input = input_item_NewExt( psz_path, psz_name, i_options,
                                                (const char **)ppsz_options,
                                                VLC_INPUT_OPTION_TRUSTED,
                                                i_duration );
                    lua_pop( L, 3 ); /* pop "path name item" */
                    /* playlist key item */
                    /* Read meta data: item must be on top of stack */
                    vlclua_read_meta_data( p_this, L, p_input );
                    /* copy the original URL to the meta data, if "URL" is still empty */
                    char* url = input_item_GetURL( p_input );
                    if( url == NULL && p_parent)
                    {
                        EnsureUTF8( psz_oldurl );
                        msg_Dbg( p_this, "meta-URL: %s", psz_oldurl );
                        input_item_SetURL ( p_input, psz_oldurl );
                    }
                    free( psz_oldurl );
                    free( url );
                    /* copy the psz_name to the meta data, if "Title" is still empty */
                    char* title = input_item_GetTitle( p_input );
                    if( title == NULL )
                        input_item_SetTitle ( p_input, psz_name );
                    free( title );
                    /* Read custom meta data: item must be on top of stack*/
                    vlclua_read_custom_meta_data( p_this, L, p_input );
                    /* Append item to playlist */
                    if( p_parent ) /* Add to node */
                    {
                        input_item_CopyOptions( p_parent, p_input );
                        input_item_node_AppendItem( p_parent_node, p_input );
                    }
                    else /* Play or Enqueue (preparse) */
                        /* FIXME: playlist_AddInput() can fail */
                        playlist_AddInput( p_playlist, p_input,
                               PLAYLIST_APPEND |
                               ( b_play ? PLAYLIST_GO : PLAYLIST_PREPARSE ),
                               PLAYLIST_END, true, false );
                    i_count ++; /* increment counter */
                    vlc_gc_decref( p_input );
                    while( i_options > 0 )
                        free( ppsz_options[--i_options] );
                    free( ppsz_options );
                    free( psz_u8path );
                }
                else
                {
                    lua_pop( L, 1 ); /* pop "path" */
                    msg_Warn( p_this,
                             "Playlist item's path should be a string" );
                }
                /* playlist key item */
            }
            else
            {
                msg_Warn( p_this, "Playlist item should be a table" );
            }
            /* <Parse playlist item> */
            lua_pop( L, 1 ); /* pop the value, keep the key for
                              * the next lua_next() call */
            /* playlist key */
        }
        /* playlist */
        if( p_parent )
        {
            if( i_count ) input_item_node_PostAndDelete( p_parent_node );
            else input_item_node_Delete( p_parent_node );
        }
    }
    else
    {
        msg_Warn( p_this, "Playlist should be a table." );
    }
    return i_count;
}
