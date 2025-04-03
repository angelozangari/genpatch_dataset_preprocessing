}
static void *Thread( void *p_data )
{
    intf_thread_t *p_intf = p_data;
    intf_sys_t *p_sys = p_intf->p_sys;
    xcb_connection_t *p_connection = p_sys->p_connection;
    int canc = vlc_savecancel();
    /* */
    xcb_flush( p_connection );
    /* */
    int fd = xcb_get_file_descriptor( p_connection );
    for( ;; )
    {
        /* Wait for x11 event */
        vlc_restorecancel( canc );
        struct pollfd fds = { .fd = fd, .events = POLLIN, };
        if( poll( &fds, 1, -1 ) < 0 )
        {
            if( errno != EINTR )
                break;
            canc = vlc_savecancel();
            continue;
        }
        canc = vlc_savecancel();
        xcb_generic_event_t *p_event;
        while( ( p_event = xcb_poll_for_event( p_connection ) ) )
        {
            if( ( p_event->response_type & 0x7f ) != XCB_KEY_PRESS )
            {
                free( p_event );
                continue;
            }
            xcb_key_press_event_t *e = (xcb_key_press_event_t *)p_event;
            for( int i = 0; i < p_sys->i_map; i++ )
            {
                hotkey_mapping_t *p_map = &p_sys->p_map[i];
                for( int j = 0; p_map->p_keys[j] != XCB_NO_SYMBOL; j++ )
                    if( p_map->p_keys[j] == e->detail &&
                        p_map->i_modifier == e->state )
                    {
                        var_SetInteger( p_intf->p_libvlc, "global-key-pressed",
                                        p_map->i_vlc );
                        goto done;
                    }
            }
        done:
            free( p_event );
        }
    }
    return NULL;
}
