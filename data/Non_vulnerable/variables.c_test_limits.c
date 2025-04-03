}
static void test_limits( libvlc_int_t *p_libvlc )
{
    vlc_value_t val;
    val.i_int = 0;
    var_Create( p_libvlc, "bla", VLC_VAR_INTEGER );
    var_Change( p_libvlc, "bla", VLC_VAR_GETMIN, &val, NULL );
    assert( val.i_int == 0 );
    val.i_int = -1234;
    var_Change( p_libvlc, "bla", VLC_VAR_SETMIN, &val, NULL );
    val.i_int = 12345;
    var_Change( p_libvlc, "bla", VLC_VAR_SETMAX, &val, NULL );
    var_Change( p_libvlc, "bla", VLC_VAR_GETMIN, &val, NULL );
    assert( val.i_int == -1234 );
    var_Change( p_libvlc, "bla", VLC_VAR_GETMAX, &val, NULL );
    assert( val.i_int == 12345 );
    var_SetInteger( p_libvlc, "bla", -123456 );
    assert( var_GetInteger( p_libvlc, "bla" ) == -1234 );
    var_SetInteger( p_libvlc, "bla", 1234 );
    assert( var_GetInteger( p_libvlc, "bla" ) == 1234 );
    var_SetInteger( p_libvlc, "bla", 12346 );
    assert( var_GetInteger( p_libvlc, "bla" ) == 12345 );
    val.i_int = 42;
    var_Change( p_libvlc, "bla", VLC_VAR_SETSTEP, &val, NULL );
    var_SetInteger( p_libvlc, "bla", 20 );
    val.i_int = 0;
    var_Change( p_libvlc, "bla", VLC_VAR_GETSTEP, &val, NULL );
    assert( val.i_int == 42 );
    var_SetInteger( p_libvlc, "bla", 20 );
    assert( var_GetInteger( p_libvlc, "bla" ) == 0 );
    var_SetInteger( p_libvlc, "bla", 21 );
    assert( var_GetInteger( p_libvlc, "bla" ) == 42 );
    var_Destroy( p_libvlc, "bla" );
}
