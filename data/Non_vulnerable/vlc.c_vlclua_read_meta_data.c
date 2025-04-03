#undef vlclua_read_meta_data
void vlclua_read_meta_data( vlc_object_t *p_this, lua_State *L,
                            input_item_t *p_input )
{
#define TRY_META( a, b )                                        \
    lua_getfield( L, -1, a );                                   \
    if( lua_isstring( L, -1 ) &&                                \
        strcmp( lua_tostring( L, -1 ), "" ) )                   \
    {                                                           \
        char *psz_value = strdup( lua_tostring( L, -1 ) );      \
        EnsureUTF8( psz_value );                                \
        msg_Dbg( p_this, #b ": %s", psz_value );                \
        input_item_Set ## b ( p_input, psz_value );             \
        free( psz_value );                                      \
    }                                                           \
    lua_pop( L, 1 ); /* pop a */
    TRY_META( "title", Title );
    TRY_META( "artist", Artist );
    TRY_META( "genre", Genre );
    TRY_META( "copyright", Copyright );
    TRY_META( "album", Album );
    TRY_META( "tracknum", TrackNum );
    TRY_META( "description", Description );
    TRY_META( "rating", Rating );
    TRY_META( "date", Date );
    TRY_META( "setting", Setting );
    TRY_META( "url", URL );
    TRY_META( "language",  Language );
    TRY_META( "nowplaying", NowPlaying );
    TRY_META( "publisher",  Publisher );
    TRY_META( "encodedby",  EncodedBy );
    TRY_META( "arturl",     ArtURL );
    TRY_META( "trackid",    TrackID );
    TRY_META( "director",   Director );
    TRY_META( "season",     Season );
    TRY_META( "episode",    Episode );
    TRY_META( "show_name",  ShowName );
    TRY_META( "actors",     Actors );
}
