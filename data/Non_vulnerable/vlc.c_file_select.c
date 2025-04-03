static const char *ppsz_lua_exts[] = { ".luac", ".lua", ".vle", NULL };
static int file_select( const char *file )
{
    int i = strlen( file );
    int j;
    for( j = 0; ppsz_lua_exts[j]; j++ )
    {
        int l = strlen( ppsz_lua_exts[j] );
        if( i >= l && !strcmp( file+i-l, ppsz_lua_exts[j] ) )
            return 1;
    }
    return 0;
}
