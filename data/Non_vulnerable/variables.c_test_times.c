}
static void test_times( libvlc_int_t *p_libvlc )
{
    int i;
    for( i = 0; i < i_var_count; i++ )
         var_Create( p_libvlc, psz_var_name[i], VLC_VAR_TIME );
    for( i = 0; i < i_var_count; i++ )
    {
        var_value[i].i_time = rand();
        var_SetTime( p_libvlc, psz_var_name[i], var_value[i].i_time );
    }
    for( i = 0; i < i_var_count; i++ )
        assert( var_GetTime( p_libvlc, psz_var_name[i] ) == var_value[i].i_time );
    for( i = 0; i < i_var_count; i++ )
        var_Destroy( p_libvlc, psz_var_name[i] );
}
