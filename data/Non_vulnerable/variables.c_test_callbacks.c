}
static void test_callbacks( libvlc_int_t *p_libvlc )
{
    /* add the callbacks */
    int i;
    for( i = 0; i < i_var_count; i++ )
    {
        var_Create( p_libvlc, psz_var_name[i], VLC_VAR_INTEGER );
        var_AddCallback( p_libvlc, psz_var_name[i], callback, psz_var_name );
    }
    /* Set the variables and trigger the callbacks */
    for( i = 0; i < i_var_count; i++ )
    {
        int i_temp = rand();
        var_SetInteger( p_libvlc, psz_var_name[i], i_temp );
        assert( i_temp == var_value[i].i_int );
        var_SetInteger( p_libvlc, psz_var_name[i], 0 );
        assert( var_value[i].i_int == 0 );
        var_value[i].i_int = 1;
    }
    /* Only trigger the callback: the value will be 0 again */
    for( i = 0; i < i_var_count; i++ )
    {
        var_TriggerCallback( p_libvlc, psz_var_name[i] );
        assert( var_value[i].i_int == 0 );
    }
    for( i = 0; i < i_var_count; i++ )
        var_Destroy( p_libvlc, psz_var_name[i] );
}
