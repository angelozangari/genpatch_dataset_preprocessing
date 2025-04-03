}
static int EventKey( vlc_object_t *p_this, char const *psz_cmd,
                        vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    decoder_t *p_dec = p_data;
    decoder_sys_t *p_sys = p_dec->p_sys;
    VLC_UNUSED(psz_cmd); VLC_UNUSED(oldval); VLC_UNUSED( p_this );
    /* FIXME: Capture + and - key for subpage browsing */
    if( newval.i_int == '-' || newval.i_int == '+' )
    {
        vlc_mutex_lock( &p_sys->lock );
        if( p_sys->i_wanted_subpage == VBI_ANY_SUBNO && newval.i_int == '+' )
            p_sys->i_wanted_subpage = vbi_dec2bcd(1);
        else if ( newval.i_int == '+' )
            p_sys->i_wanted_subpage = vbi_add_bcd( p_sys->i_wanted_subpage, 1);
        else if( newval.i_int == '-')
            p_sys->i_wanted_subpage = vbi_add_bcd( p_sys->i_wanted_subpage, 0xF9999999); /* BCD complement - 1 */
        if ( !vbi_bcd_digits_greater( p_sys->i_wanted_subpage, 0x00 ) || vbi_bcd_digits_greater( p_sys->i_wanted_subpage, 0x99 ) )
                p_sys->i_wanted_subpage = VBI_ANY_SUBNO;
        else
            msg_Info( p_dec, "subpage: %d",
                      vbi_bcd2dec( p_sys->i_wanted_subpage) );
        p_sys->b_update = true;
        vlc_mutex_unlock( &p_sys->lock );
    }
    /* Capture 0-9 for page selection */
    if( newval.i_int < '0' || newval.i_int > '9' )
        return VLC_SUCCESS;
    vlc_mutex_lock( &p_sys->lock );
    p_sys->i_key[0] = p_sys->i_key[1];
    p_sys->i_key[1] = p_sys->i_key[2];
    p_sys->i_key[2] = (int)(newval.i_int - '0');
    msg_Info( p_dec, "page: %c%c%c", (char)(p_sys->i_key[0]+'0'),
              (char)(p_sys->i_key[1]+'0'), (char)(p_sys->i_key[2]+'0') );
    int i_new_page = 0;
    if( p_sys->i_key[0] > 0 && p_sys->i_key[0] <= 8 &&
        p_sys->i_key[1] >= 0 && p_sys->i_key[1] <= 9 &&
        p_sys->i_key[2] >= 0 && p_sys->i_key[2] <= 9 )
    {
        i_new_page = p_sys->i_key[0]*100 + p_sys->i_key[1]*10 + p_sys->i_key[2];
        p_sys->i_key[0] = p_sys->i_key[1] = p_sys->i_key[2] = '*' - '0';
    }
    vlc_mutex_unlock( &p_sys->lock );
    if( i_new_page > 0 )
        var_SetInteger( p_dec, "vbi-page", i_new_page );
    return VLC_SUCCESS;
}
