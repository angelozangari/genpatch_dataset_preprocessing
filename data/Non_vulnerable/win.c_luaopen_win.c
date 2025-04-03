};
void luaopen_win( lua_State *L )
{
    lua_newtable( L );
    luaL_register( L, NULL, vlclua_win_reg );
    lua_setfield( L, -2, "win" );
}
