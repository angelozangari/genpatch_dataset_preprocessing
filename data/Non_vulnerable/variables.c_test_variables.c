}
static void test_variables( libvlc_instance_t *p_vlc )
{
    libvlc_int_t *p_libvlc = p_vlc->p_libvlc_int;
    srand( time( NULL ) );
    log( "Testing for integers\n" );
    test_integer( p_libvlc );
    log( "Testing for booleans\n" );
    test_booleans( p_libvlc );
    log( "Testing for times\n" );
    test_times( p_libvlc );
    log( "Testing for floats\n" );
    test_floats( p_libvlc );
    log( "Testing for strings\n" );
    test_strings( p_libvlc );
    log( "Testing for addresses\n" );
    test_address( p_libvlc );
    log( "Testing the callbacks\n" );
    test_callbacks( p_libvlc );
    log( "Testing the limits\n" );
    test_limits( p_libvlc );
    log( "Testing choices\n" );
    test_choices( p_libvlc );
    log( "Testing var_Change()\n" );
    test_change( p_libvlc );
    log( "Testing type at creation\n" );
    test_creation_and_type( p_libvlc );
}
