}
static void Register( intf_thread_t *p_intf )
{
    intf_sys_t *p_sys = p_intf->p_sys;
    for( int i = 0; i < p_sys->i_map; i++ )
    {
        const hotkey_mapping_t *p_map = &p_sys->p_map[i];
        for( int j = 0; p_map->p_keys[j] != XCB_NO_SYMBOL; j++ )
        {
            xcb_grab_key( p_sys->p_connection, true, p_sys->root,
                          p_map->i_modifier, p_map->p_keys[j],
                          XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC );
        }
    }
}
