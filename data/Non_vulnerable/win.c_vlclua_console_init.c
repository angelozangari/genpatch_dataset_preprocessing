}
static int vlclua_console_init( lua_State *L )
{
    (void)L;
    //if ( !AllocConsole() )
    //    luaL_error( L, "failed to allocate windows console" );
    AllocConsole();
    freopen( "CONOUT$", "w", stdout );
    freopen( "CONOUT$", "w", stderr );
    freopen( "CONIN$", "r", stdin );
    return 0;
}
