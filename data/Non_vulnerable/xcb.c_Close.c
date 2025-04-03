 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    intf_thread_t *p_intf = (intf_thread_t *)p_this;
    intf_sys_t *p_sys = p_intf->p_sys;
    if( !p_sys )
        return; /* if we were running disabled */
    vlc_cancel( p_sys->thread );
    vlc_join( p_sys->thread, NULL );
    if( p_sys->p_map )
    {
        free( p_sys->p_map->p_keys );
        free( p_sys->p_map );
    }
    xcb_key_symbols_free( p_sys->p_symbols );
    xcb_disconnect( p_sys->p_connection );
    free( p_sys );
}
