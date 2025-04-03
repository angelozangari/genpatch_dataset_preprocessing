 *****************************************************************************/
LRESULT CALLBACK WMHOTKEYPROC( HWND hwnd, UINT uMsg, WPARAM wParam,
        LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_HOTKEY:
            {
                char psz_atomName[44];
                LONG_PTR ret = GetWindowLongPtr( hwnd, GWLP_USERDATA );
                intf_thread_t *p_intf = (intf_thread_t*)ret;
                strcpy( psz_atomName, "key-" );
                if( !GlobalGetAtomNameA(
                        wParam, psz_atomName + 4,
                        sizeof( psz_atomName ) - 4 ) )
                    return 0;
                /* search for key associated with VLC */
                vlc_action_t action = vlc_GetActionId( psz_atomName );
                if( action != ACTIONID_NONE )
                {
                    var_SetInteger( p_intf->p_libvlc,
                            "key-action", action );
                    return 1;
                }
            }
            break;
        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;
        default:
            return DefWindowProc( hwnd, uMsg, wParam, lParam );
    }
    return 0;
}
