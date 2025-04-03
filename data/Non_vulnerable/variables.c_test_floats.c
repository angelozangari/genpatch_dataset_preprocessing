}
static void test_floats( libvlc_int_t *p_libvlc )
{
    int i;
    for( i = 0; i < i_var_count; i++ )
         var_Create( p_libvlc, psz_var_name[i], VLC_VAR_FLOAT );
    for( i = 0; i < i_var_count; i++ )
    {
        var_value[i].f_float = rand();
        var_SetFloat( p_libvlc, psz_var_name[i], var_value[i].f_float );
    }
    for( i = 0; i < i_var_count; i++ )
        assert( var_GetFloat( p_libvlc, psz_var_name[i] ) == var_value[i].f_float );
    for( i = 0; i < i_var_count; i++ )
        var_Destroy( p_libvlc, psz_var_name[i] );
}
