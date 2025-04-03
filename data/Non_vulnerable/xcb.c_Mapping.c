}
static bool Mapping( intf_thread_t *p_intf )
{
    static const xcb_keysym_t p_x11_modifier_ignored[] = {
        0,
        XK_Num_Lock,
        XK_Scroll_Lock,
        XK_Caps_Lock,
    };
    intf_sys_t *p_sys = p_intf->p_sys;
    bool active = false;
    p_sys->i_map = 0;
    p_sys->p_map = NULL;
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
        uint_fast32_t i_vlc_key = vlc_str2keycode( key );
        free( key );
        if( i_vlc_key == KEY_UNSET )
            continue;
        xcb_keycode_t *p_keys = xcb_key_symbols_get_keycode(
                p_sys->p_symbols, GetX11Key( i_vlc_key & ~KEY_MODIFIER ) );
        if( !p_keys )
            continue;
        const unsigned i_modifier = GetX11Modifier( p_sys->p_connection,
                p_sys->p_symbols, i_vlc_key & KEY_MODIFIER );
        const size_t max = sizeof(p_x11_modifier_ignored) /
                sizeof(*p_x11_modifier_ignored);
        for( unsigned int i = 0; i < max; i++ )
        {
            const unsigned i_ignored = GetModifier( p_sys->p_connection,
                    p_sys->p_symbols, p_x11_modifier_ignored[i] );
            if( i != 0 && i_ignored == 0)
                continue;
            hotkey_mapping_t *p_map_old = p_sys->p_map;
            p_sys->p_map = realloc( p_sys->p_map,
                    sizeof(*p_sys->p_map) * (p_sys->i_map+1) );
            if( !p_sys->p_map )
            {
                p_sys->p_map = p_map_old;
                break;
            }
            hotkey_mapping_t *p_map = &p_sys->p_map[p_sys->i_map++];
            p_map->p_keys = p_keys;
            p_map->i_modifier = i_modifier|i_ignored;
            p_map->i_vlc = i_vlc_key;
            active = true;
        }
    }
    return active;
}
