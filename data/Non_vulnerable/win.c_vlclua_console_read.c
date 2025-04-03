}
static int vlclua_console_read( lua_State *L )
{
    char psz_buffer[MAX_LINE_LENGTH+1];
    int i_size = 0;
    ReadWin32( GetConsole( L ), psz_buffer, &i_size );
    lua_pushlstring( L, psz_buffer, i_size );
    return 1;
}
