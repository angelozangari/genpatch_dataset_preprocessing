/* Based on modules/control/rc.c and include/vlc_interface.h */
static HANDLE GetConsole( lua_State *L )
{
    /* Get the file descriptor of the console input */
    HANDLE hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
    if( hConsoleIn == INVALID_HANDLE_VALUE )
        luaL_error( L, "couldn't find user input handle" );
    return hConsoleIn;
}
