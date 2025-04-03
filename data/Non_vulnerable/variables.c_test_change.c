}
static void test_change( libvlc_int_t *p_libvlc )
{
    /* Add min, max and step
       Yes we can have min > max but we don't really care */
    vlc_value_t val;
    int i_min, i_max, i_step;
    var_Create( p_libvlc, "bla", VLC_VAR_INTEGER );
    val.i_int = i_min = rand();
    var_Change( p_libvlc, "bla", VLC_VAR_SETMIN, &val, NULL );
    val.i_int = i_max = rand();
    var_Change( p_libvlc, "bla", VLC_VAR_SETMAX, &val, NULL );
    val.i_int = i_step = rand();
    var_Change( p_libvlc, "bla", VLC_VAR_SETSTEP, &val, NULL );
    /* Do something */
    var_SetInteger( p_libvlc, "bla", rand() );
    val.i_int = var_GetInteger( p_libvlc, "bla" ); /* dummy read */
    /* Test everything is right */
    var_Change( p_libvlc, "bla", VLC_VAR_GETMIN, &val, NULL );
    assert( val.i_int == i_min );
    var_Change( p_libvlc, "bla", VLC_VAR_GETMAX, &val, NULL );
    assert( val.i_int == i_max );
    var_Change( p_libvlc, "bla", VLC_VAR_GETSTEP, &val, NULL );
    assert( val.i_int == i_step );
    var_Destroy( p_libvlc, "bla" );
}
