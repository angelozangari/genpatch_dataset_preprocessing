 *****************************************************************************/
static void *Thread( void *p_data )
{
    MSG message;
    intf_thread_t *p_intf = p_data;
    intf_sys_t *p_sys = p_intf->p_sys;
    /* Window which receives Hotkeys */
    vlc_mutex_lock( &p_sys->lock );
    p_sys->hotkeyWindow =
        (void*)CreateWindow( _T("STATIC"),           /* name of window class */
                _T("VLC ghk ") _T(VERSION),         /* window title bar text */
                0,                                           /* window style */
                0,                                   /* default X coordinate */
                0,                                   /* default Y coordinate */
                0,                                           /* window width */
                0,                                          /* window height */
                NULL,                                    /* no parent window */
                NULL,                              /* no menu in this window */
                GetModuleHandle(NULL),    /* handle of this program instance */
                NULL );                                 /* sent to WM_CREATE */
    if( p_sys->hotkeyWindow == NULL )
    {
        p_sys->hotkeyWindow = INVALID_HANDLE_VALUE;
        vlc_cond_signal( &p_sys->wait );
        vlc_mutex_unlock( &p_sys->lock );
        return NULL;
    }
    vlc_cond_signal( &p_sys->wait );
    vlc_mutex_unlock( &p_sys->lock );
    SetWindowLongPtr( p_sys->hotkeyWindow, GWLP_WNDPROC,
            (LONG_PTR)WMHOTKEYPROC );
    SetWindowLongPtr( p_sys->hotkeyWindow, GWLP_USERDATA,
            (LONG_PTR)p_intf );
    /* Registering of Hotkeys */
    for( const struct hotkey *p_hotkey = p_intf->p_libvlc->p_hotkeys;
            p_hotkey->psz_action != NULL;
            p_hotkey++ )
    {
        char varname[12 + strlen( p_hotkey->psz_action )];
        sprintf( varname, "global-key-%s", p_hotkey->psz_action );
        char *key = var_InheritString( p_intf, varname );
        if( key == NULL )
            continue;
        UINT i_key = vlc_str2keycode( key );
        free( key );
        if( i_key == KEY_UNSET )
            continue;
        UINT i_keyMod = 0;
        if( i_key & KEY_MODIFIER_SHIFT ) i_keyMod |= MOD_SHIFT;
        if( i_key & KEY_MODIFIER_ALT ) i_keyMod |= MOD_ALT;
        if( i_key & KEY_MODIFIER_CTRL ) i_keyMod |= MOD_CONTROL;
#define HANDLE( key ) case KEY_##key: i_vk = VK_##key; break
#define HANDLE2( key, key2 ) case KEY_##key: i_vk = VK_##key2; break
#define KEY_SPACE ' '
#ifndef VK_VOLUME_DOWN
#define VK_VOLUME_DOWN          0xAE
#define VK_VOLUME_UP            0xAF
#endif
#ifndef VK_MEDIA_NEXT_TRACK
#define VK_MEDIA_NEXT_TRACK     0xB0
#define VK_MEDIA_PREV_TRACK     0xB1
#define VK_MEDIA_STOP           0xB2
#define VK_MEDIA_PLAY_PAUSE     0xB3
#endif
#ifndef VK_PAGEUP
#define VK_PAGEUP               0x21
#define VK_PAGEDOWN             0x22
#endif
        UINT i_vk = 0;
        switch( i_key & ~KEY_MODIFIER )
        {
            HANDLE( LEFT );
            HANDLE( RIGHT );
            HANDLE( UP );
            HANDLE( DOWN );
            HANDLE( SPACE );
            HANDLE2( ESC, ESCAPE );
            HANDLE2( ENTER, RETURN );
            HANDLE( F1 );
            HANDLE( F2 );
            HANDLE( F3 );
            HANDLE( F4 );
            HANDLE( F5 );
            HANDLE( F6 );
            HANDLE( F7 );
            HANDLE( F8 );
            HANDLE( F9 );
            HANDLE( F10 );
            HANDLE( F11 );
            HANDLE( F12 );
            HANDLE( PAGEUP );
            HANDLE( PAGEDOWN );
            HANDLE( HOME );
            HANDLE( END );
            HANDLE( INSERT );
            HANDLE( DELETE );
            HANDLE( VOLUME_DOWN );
            HANDLE( VOLUME_UP );
            HANDLE( MEDIA_PLAY_PAUSE );
            HANDLE( MEDIA_STOP );
            HANDLE( MEDIA_PREV_TRACK );
            HANDLE( MEDIA_NEXT_TRACK );
            default:
                i_vk = toupper( (uint8_t)(i_key & ~KEY_MODIFIER) );
                break;
        }
        if( !i_vk ) continue;
#undef HANDLE
#undef HANDLE2
        ATOM atom = GlobalAddAtomA( p_hotkey->psz_action );
        if( !atom ) continue;
        if( !RegisterHotKey( p_sys->hotkeyWindow, atom, i_keyMod, i_vk ) )
            GlobalDeleteAtom( atom );
    }
    /* Main message loop */
    while( GetMessage( &message, NULL, 0, 0 ) )
        DispatchMessage( &message );
    /* Unregistering of Hotkeys */
    for( const struct hotkey *p_hotkey = p_intf->p_libvlc->p_hotkeys;
            p_hotkey->psz_action != NULL;
            p_hotkey++ )
    {
        ATOM atom = GlobalFindAtomA( p_hotkey->psz_action );
        if( !atom ) continue;
        if( UnregisterHotKey( p_sys->hotkeyWindow, atom ) )
            GlobalDeleteAtom( atom );
    }
    /* close window */
    vlc_mutex_lock( &p_sys->lock );
    DestroyWindow( p_sys->hotkeyWindow );
    p_sys->hotkeyWindow = NULL;
    vlc_mutex_unlock( &p_sys->lock );
    return NULL;
}
