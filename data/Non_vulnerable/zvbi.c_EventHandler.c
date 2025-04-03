}
static void EventHandler( vbi_event *ev, void *user_data )
{
    decoder_t *p_dec        = (decoder_t *)user_data;
    decoder_sys_t *p_sys    = p_dec->p_sys;
    if( ev->type == VBI_EVENT_TTX_PAGE )
    {
#ifdef ZVBI_DEBUG
        msg_Info( p_dec, "Page %03x.%02x ",
                    ev->ev.ttx_page.pgno,
                    ev->ev.ttx_page.subno & 0xFF);
#endif
        if( p_sys->i_last_page == vbi_bcd2dec( ev->ev.ttx_page.pgno ) )
            p_sys->b_update = true;
#ifdef ZVBI_DEBUG
        if( ev->ev.ttx_page.clock_update )
            msg_Dbg( p_dec, "clock" );
        if( ev->ev.ttx_page.header_update )
            msg_Dbg( p_dec, "header" );
#endif
    }
    else if( ev->type == VBI_EVENT_CLOSE )
        msg_Dbg( p_dec, "Close event" );
    else if( ev->type == VBI_EVENT_CAPTION )
        msg_Dbg( p_dec, "Caption line: %x", ev->ev.caption.pgno );
    else if( ev->type == VBI_EVENT_NETWORK )
    {
        msg_Dbg( p_dec, "Network change");
        vbi_network n = ev->ev.network;
        msg_Dbg( p_dec, "Network id:%d name: %s, call: %s ", n.nuid, n.name, n.call );
    }
    else if( ev->type == VBI_EVENT_TRIGGER )
        msg_Dbg( p_dec, "Trigger event" );
    else if( ev->type == VBI_EVENT_ASPECT )
        msg_Dbg( p_dec, "Aspect update" );
    else if( ev->type == VBI_EVENT_PROG_INFO )
        msg_Dbg( p_dec, "Program info received" );
    else if( ev->type == VBI_EVENT_NETWORK_ID )
        msg_Dbg( p_dec, "Network ID changed" );
}
