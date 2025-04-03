}
int main( void )
{
    libvlc_instance_t *p_vlc;
    test_init();
    log( "Testing the core variables\n" );
    p_vlc = libvlc_new( test_defaults_nargs, test_defaults_args );
    assert( p_vlc != NULL );
    test_variables( p_vlc );
    libvlc_release( p_vlc );
    return 0;
}
