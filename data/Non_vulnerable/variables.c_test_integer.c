vlc_value_t var_value[6];
static void test_integer( libvlc_int_t *p_libvlc )
{
    int i;
    for( i = 0; i < i_var_count; i++ )
        var_Create( p_libvlc, psz_var_name[i], VLC_VAR_INTEGER );
    for( i = 0; i < i_var_count; i++ )
    {
        var_value[i].i_int = rand();
        var_SetInteger( p_libvlc, psz_var_name[i], var_value[i].i_int );
    }
    for( i = 0; i < i_var_count; i++ )
    {
        assert( var_GetInteger( p_libvlc, psz_var_name[i] ) == var_value[i].i_int );
        var_IncInteger( p_libvlc, psz_var_name[i] );
        assert( var_GetInteger( p_libvlc, psz_var_name[i] ) == var_value[i].i_int + 1 );
        var_DecInteger( p_libvlc, psz_var_name[i] );
        assert( var_GetInteger( p_libvlc, psz_var_name[i] ) == var_value[i].i_int );
    }
    for( i = 0; i < i_var_count; i++ )
        var_Destroy( p_libvlc, psz_var_name[i] );
}
