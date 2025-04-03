}
static void test_strings( libvlc_int_t *p_libvlc )
{
    int i;
    char *psz_tmp;
    for( i = 0; i < i_var_count; i++ )
         var_Create( p_libvlc, psz_var_name[i], VLC_VAR_STRING );
    for( i = 0; i < i_var_count; i++ )
        var_SetString( p_libvlc, psz_var_name[i], psz_var_name[i] );
    for( i = 0; i < i_var_count; i++ )
    {
        psz_tmp = var_GetString( p_libvlc, psz_var_name[i] );
        assert( !strcmp( psz_tmp, psz_var_name[i] ) );
        free( psz_tmp );
    }
    for( i = 0; i < i_var_count; i++ )
        var_Destroy( p_libvlc, psz_var_name[i] );
    /* Some more test for strings */
    var_Create( p_libvlc, "bla", VLC_VAR_STRING );
    assert( var_GetNonEmptyString( p_libvlc, "bla" ) == NULL );
    var_SetString( p_libvlc, "bla", "" );
    assert( var_GetNonEmptyString( p_libvlc, "bla" ) == NULL );
    var_SetString( p_libvlc, "bla", "test" );
    psz_tmp = var_GetNonEmptyString( p_libvlc, "bla" );
    assert( !strcmp( psz_tmp, "test" ) );
    free( psz_tmp );
    var_Destroy( p_libvlc, "bla" );
}
