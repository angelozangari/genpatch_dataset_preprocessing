}
void vlclua_dir_list_free( char **ppsz_dir_list )
{
    for( char **ppsz_dir = ppsz_dir_list; *ppsz_dir; ppsz_dir++ )
        free( *ppsz_dir );
    free( ppsz_dir_list );
}
