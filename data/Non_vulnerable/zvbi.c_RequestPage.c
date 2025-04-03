/* Callbacks */
static int RequestPage( vlc_object_t *p_this, char const *psz_cmd,
                        vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    decoder_sys_t *p_sys = p_data;
    VLC_UNUSED(p_this); VLC_UNUSED(psz_cmd); VLC_UNUSED(oldval);
    vlc_mutex_lock( &p_sys->lock );
    switch( newval.i_int )
    {
        case ZVBI_KEY_RED:
            p_sys->i_wanted_page = vbi_bcd2dec( p_sys->nav_link[0].pgno );
            p_sys->i_wanted_subpage = p_sys->nav_link[0].subno;
            break;
        case ZVBI_KEY_GREEN:
            p_sys->i_wanted_page = vbi_bcd2dec( p_sys->nav_link[1].pgno );
            p_sys->i_wanted_subpage = p_sys->nav_link[1].subno;
            break;
        case ZVBI_KEY_YELLOW:
            p_sys->i_wanted_page = vbi_bcd2dec( p_sys->nav_link[2].pgno );
            p_sys->i_wanted_subpage = p_sys->nav_link[2].subno;
            break;
        case ZVBI_KEY_BLUE:
            p_sys->i_wanted_page = vbi_bcd2dec( p_sys->nav_link[3].pgno );
            p_sys->i_wanted_subpage = p_sys->nav_link[3].subno;
            break;
        case ZVBI_KEY_INDEX:
            p_sys->i_wanted_page = vbi_bcd2dec( p_sys->nav_link[5].pgno ); /* #4 is SKIPPED */
            p_sys->i_wanted_subpage = p_sys->nav_link[5].subno;
            break;
    }
    if( newval.i_int > 0 && newval.i_int < 999 )
    {
        p_sys->i_wanted_page = newval.i_int;
        p_sys->i_wanted_subpage = VBI_ANY_SUBNO;
    }
    vlc_mutex_unlock( &p_sys->lock );
    return VLC_SUCCESS;
}
