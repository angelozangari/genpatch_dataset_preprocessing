}
static void test_address( libvlc_int_t *p_libvlc )
{
    char dummy[i_var_count];
    int i;
    for( i = 0; i < i_var_count; i++ )
         var_Create( p_libvlc, psz_var_name[i], VLC_VAR_ADDRESS );
    for( i = 0; i < i_var_count; i++ )
    {
        var_value[i].p_address = dummy + i;
        var_SetAddress( p_libvlc, psz_var_name[i], var_value[i].p_address );
    }
    for( i = 0; i < i_var_count; i++ )
    {
        vlc_value_t val;
        var_Get( p_libvlc, psz_var_name[i], &val );
        assert( val.p_address == var_value[i].p_address );
    }
    for( i = 0; i < i_var_count; i++ )
        var_Destroy( p_libvlc, psz_var_name[i] );
}
