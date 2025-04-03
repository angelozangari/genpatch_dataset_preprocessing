 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    intf_thread_t *p_intf = (intf_thread_t *)p_this;
    intf_sys_t *p_sys;
    int ret = VLC_EGENERIC;
    p_intf->p_sys = p_sys = calloc( 1, sizeof(*p_sys) );
    if( !p_sys )
        return VLC_ENOMEM;
    int i_screen_default;
    p_sys->p_connection = xcb_connect( NULL, &i_screen_default );
    if( xcb_connection_has_error( p_sys->p_connection ) )
        goto error;
    /* Get the root windows of the default screen */
    const xcb_setup_t* xcbsetup = xcb_get_setup( p_sys->p_connection );
    if( !xcbsetup )
        goto error;
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator( xcbsetup );
    for( int i = 0; i < i_screen_default; i++ )
    {
        if( !iter.rem )
            break;
        xcb_screen_next( &iter );
    }
    if( !iter.rem )
        goto error;
    p_sys->root = iter.data->root;
    /* */
    p_sys->p_symbols = xcb_key_symbols_alloc( p_sys->p_connection ); // FIXME
    if( !p_sys->p_symbols )
        goto error;
    if( !Mapping( p_intf ) )
    {
        ret = VLC_SUCCESS;
        p_intf->p_sys = NULL; /* for Close() */
        goto error;
    }
    Register( p_intf );
    if( vlc_clone( &p_sys->thread, Thread, p_intf, VLC_THREAD_PRIORITY_LOW ) )
    {
        if( p_sys->p_map )
        {
            free( p_sys->p_map->p_keys );
            free( p_sys->p_map );
        }
        goto error;
    }
    return VLC_SUCCESS;
error:
    if( p_sys->p_symbols )
        xcb_key_symbols_free( p_sys->p_symbols );
    xcb_disconnect( p_sys->p_connection );
    free( p_sys );
    return ret;
}
