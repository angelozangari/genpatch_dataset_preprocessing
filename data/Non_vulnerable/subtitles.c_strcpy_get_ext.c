}
static void strcpy_get_ext( char *d, const char *s )
{
    const char *tmp = strrchr(s, '.');
    if( !tmp )
        strcpy(d, "");
    else
        strcpy( d, tmp + 1 );
}
