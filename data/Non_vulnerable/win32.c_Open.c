 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    intf_thread_t *p_intf = (intf_thread_t *)p_this;
    intf_sys_t *p_sys = malloc( sizeof (intf_sys_t) );
    if( p_sys == NULL )
        return VLC_ENOMEM;
    p_intf->p_sys = p_sys;
    p_sys->hotkeyWindow = NULL;
    vlc_mutex_init( &p_sys->lock );
    vlc_cond_init( &p_sys->wait );
    if( vlc_clone( &p_sys->thread, Thread, p_intf, VLC_THREAD_PRIORITY_LOW ) )
    {
        vlc_mutex_destroy( &p_sys->lock );
        vlc_cond_destroy( &p_sys->wait );
        free( p_sys );
        p_intf->p_sys = NULL;
        return VLC_ENOMEM;
    }
    vlc_mutex_lock( &p_sys->lock );
    while( p_sys->hotkeyWindow == NULL )
        vlc_cond_wait( &p_sys->wait, &p_sys->lock );
    if( p_sys->hotkeyWindow == INVALID_HANDLE_VALUE )
    {
        vlc_mutex_unlock( &p_sys->lock );
        vlc_join( p_sys->thread, NULL );
        vlc_mutex_destroy( &p_sys->lock );
        vlc_cond_destroy( &p_sys->wait );
        free( p_sys );
        p_intf->p_sys = NULL;
        return VLC_ENOMEM;
    }
    vlc_mutex_unlock( &p_sys->lock );
    return VLC_SUCCESS;
}
