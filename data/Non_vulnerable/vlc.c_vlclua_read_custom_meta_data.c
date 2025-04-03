#undef vlclua_read_custom_meta_data
void vlclua_read_custom_meta_data( vlc_object_t *p_this, lua_State *L,
                                     input_item_t *p_input )
{
    /* Lock the input item and create the meta table if needed */
    vlc_mutex_lock( &p_input->lock );
    if( !p_input->p_meta )
        p_input->p_meta = vlc_meta_New();
    /* ... item */
    lua_getfield( L, -1, "meta" );
    /* ... item meta */
    if( lua_istable( L, -1 ) )
    {
        lua_pushnil( L );
        /* ... item meta nil */
        while( lua_next( L, -2 ) )
        {
            /* ... item meta key value */
            if( !lua_isstring( L, -2 ) || !lua_isstring( L, -1 ) )
            {
                msg_Err( p_this, "'meta' keys and values must be strings");
                lua_pop( L, 1 ); /* pop "value" */
                continue;
            }
            const char *psz_key = lua_tostring( L, -2 );
            const char *psz_value = lua_tostring( L, -1 );
            vlc_meta_AddExtra( p_input->p_meta, psz_key, psz_value );
            lua_pop( L, 1 ); /* pop "value" */
        }
    }
    lua_pop( L, 1 ); /* pop "meta" */
    /* ... item -> back to original stack */
    vlc_mutex_unlock( &p_input->lock );
}
