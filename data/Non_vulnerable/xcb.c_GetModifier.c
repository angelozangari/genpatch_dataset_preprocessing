 *****************************************************************************/
static unsigned GetModifier( xcb_connection_t *p_connection, xcb_key_symbols_t *p_symbols, xcb_keysym_t sym )
{
    static const unsigned pi_mask[8] = {
        XCB_MOD_MASK_SHIFT, XCB_MOD_MASK_LOCK, XCB_MOD_MASK_CONTROL,
        XCB_MOD_MASK_1, XCB_MOD_MASK_2, XCB_MOD_MASK_3,
        XCB_MOD_MASK_4, XCB_MOD_MASK_5
    };
    if( sym == 0 )
        return 0; /* no modifier */
    xcb_get_modifier_mapping_cookie_t r =
            xcb_get_modifier_mapping( p_connection );
    xcb_get_modifier_mapping_reply_t *p_map =
            xcb_get_modifier_mapping_reply( p_connection, r, NULL );
    if( !p_map )
        return 0;
    xcb_keycode_t *p_keys = xcb_key_symbols_get_keycode( p_symbols, sym );
    if( !p_keys )
        goto end;
    int i = 0;
    bool no_modifier = true;
    while( p_keys[i] != XCB_NO_SYMBOL )
    {
        if( p_keys[i] != 0 )
        {
            no_modifier = false;
            break;
        }
        i++;
    }
    if( no_modifier )
        goto end;
    xcb_keycode_t *p_keycode = xcb_get_modifier_mapping_keycodes( p_map );
    if( !p_keycode )
        goto end;
    for( int i = 0; i < 8; i++ )
        for( int j = 0; j < p_map->keycodes_per_modifier; j++ )
            for( int k = 0; p_keys[k] != XCB_NO_SYMBOL; k++ )
                if( p_keycode[i*p_map->keycodes_per_modifier + j] == p_keys[k])
                {
                    free( p_keys );
                    free( p_map );
                    return pi_mask[i];
                }
end:
    free( p_keys );
    free( p_map ); // FIXME to check
    return 0;
}
