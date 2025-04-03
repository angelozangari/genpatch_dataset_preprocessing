 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    intf_thread_t *p_intf = (intf_thread_t *)p_this;
    intf_sys_t *p_sys = p_intf->p_sys;
    /* stop hotkey window */
    vlc_mutex_lock( &p_sys->lock );
    if( p_sys->hotkeyWindow != NULL )
        PostMessage( p_sys->hotkeyWindow, WM_CLOSE, 0, 0 );
    vlc_mutex_unlock( &p_sys->lock );
    vlc_join( p_sys->thread, NULL );
    vlc_mutex_destroy( &p_sys->lock );
    vlc_cond_destroy( &p_sys->wait );
    free( p_sys );
}
