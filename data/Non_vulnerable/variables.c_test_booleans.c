}
static void test_booleans( libvlc_int_t *p_libvlc )
{
    int i;
    for( i = 0; i < i_var_count; i++ )
         var_Create( p_libvlc, psz_var_name[i], VLC_VAR_BOOL );
    for( i = 0; i < i_var_count; i++ )
    {
        var_value[i].b_bool = (rand() > RAND_MAX/2);
        var_SetBool( p_libvlc, psz_var_name[i], var_value[i].b_bool );
    }
    for( i = 0; i < i_var_count; i++ )
        assert( var_GetBool( p_libvlc, psz_var_name[i] ) == var_value[i].b_bool );
    for( i = 0; i < i_var_count; i++ )
        var_ToggleBool( p_libvlc, psz_var_name[i] );
    for( i = 0; i < i_var_count; i++ )
        assert( var_GetBool( p_libvlc, psz_var_name[i] ) != var_value[i].b_bool );
    for( i = 0; i < i_var_count; i++ )
        var_Destroy( p_libvlc, psz_var_name[i] );
}
