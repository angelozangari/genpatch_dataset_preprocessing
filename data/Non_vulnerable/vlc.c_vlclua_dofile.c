/** Replacement for luaL_dofile, using VLC's input capabilities */
int vlclua_dofile( vlc_object_t *p_this, lua_State *L, const char *curi )
{
    char *uri = ToLocaleDup( curi );
    if( !strstr( uri, "://" ) ) {
        int ret = luaL_dofile( L, uri );
        free( uri );
        return ret;
    }
    if( !strncasecmp( uri, "file://", 7 ) ) {
        int ret = luaL_dofile( L, uri + 7 );
        free( uri );
        return ret;
    }
    stream_t *s = stream_UrlNew( p_this, uri );
    if( !s )
    {
        free( uri );
        return 1;
    }
    int64_t i_size = stream_Size( s );
    char *p_buffer = ( i_size > 0 ) ? malloc( i_size ) : NULL;
    if( !p_buffer )
    {
        // FIXME: read the whole stream until we reach the end (if no size)
        stream_Delete( s );
        free( uri );
        return 1;
    }
    int64_t i_read = stream_Read( s, p_buffer, (int) i_size );
    int i_ret = ( i_read == i_size ) ? 0 : 1;
    if( !i_ret )
        i_ret = luaL_loadbuffer( L, p_buffer, (size_t) i_size, uri );
    if( !i_ret )
        i_ret = lua_pcall( L, 0, LUA_MULTRET, 0 );
    stream_Delete( s );
    free( p_buffer );
    free( uri );
    return i_ret;
}
