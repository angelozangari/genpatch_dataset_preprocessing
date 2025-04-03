}
static unsigned GetX11Modifier( xcb_connection_t *p_connection,
        xcb_key_symbols_t *p_symbols, unsigned i_vlc )
{
    unsigned i_mask = 0;
    if( i_vlc & KEY_MODIFIER_ALT )
        i_mask |= GetModifier( p_connection, p_symbols, XK_Alt_L ) |
                  GetModifier( p_connection, p_symbols, XK_Alt_R );
    if( i_vlc & KEY_MODIFIER_SHIFT )
        i_mask |= GetModifier( p_connection, p_symbols, XK_Shift_L ) |
                  GetModifier( p_connection, p_symbols, XK_Shift_R );
    if( i_vlc & KEY_MODIFIER_CTRL )
        i_mask |= GetModifier( p_connection, p_symbols, XK_Control_L ) |
                  GetModifier( p_connection, p_symbols, XK_Control_R );
    if( i_vlc & KEY_MODIFIER_META )
        i_mask |= GetModifier( p_connection, p_symbols, XK_Meta_L ) |
                  GetModifier( p_connection, p_symbols, XK_Meta_R ) |
                  GetModifier( p_connection, p_symbols, XK_Super_L ) |
                  GetModifier( p_connection, p_symbols, XK_Super_R );
    return i_mask;
}
