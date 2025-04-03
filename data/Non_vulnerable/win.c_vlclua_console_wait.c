}
static int vlclua_console_wait( lua_State *L )
{
    int i_timeout = luaL_optint( L, 1, 0 );
    DWORD status = WaitForSingleObject( GetConsole( L ), i_timeout );
    lua_pushboolean( L, status == WAIT_OBJECT_0 );
    return 1;
}
