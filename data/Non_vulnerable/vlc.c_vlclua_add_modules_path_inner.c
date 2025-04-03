}
static int vlclua_add_modules_path_inner( lua_State *L, const char *psz_path )
{
    int count = 0;
    for( const char **ppsz_ext = ppsz_lua_exts; *ppsz_ext; ppsz_ext++ )
    {
        lua_pushfstring( L, "%s"DIR_SEP"modules"DIR_SEP"?%s;",
                         psz_path, *ppsz_ext );
        count ++;
    }
    return count;
}
