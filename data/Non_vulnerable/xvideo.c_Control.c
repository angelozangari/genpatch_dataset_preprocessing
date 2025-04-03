}
static int Control (vout_display_t *vd, int query, va_list ap)
{
    vout_display_sys_t *p_sys = vd->sys;
    switch (query)
    {
    case VOUT_DISPLAY_CHANGE_FULLSCREEN:
    {
        const vout_display_cfg_t *c = va_arg (ap, const vout_display_cfg_t *);
        return vout_window_SetFullScreen (p_sys->embed, c->is_fullscreen);
    }
    case VOUT_DISPLAY_CHANGE_DISPLAY_SIZE:
    case VOUT_DISPLAY_CHANGE_DISPLAY_FILLED:
    case VOUT_DISPLAY_CHANGE_ZOOM:
    case VOUT_DISPLAY_CHANGE_SOURCE_ASPECT:
    case VOUT_DISPLAY_CHANGE_SOURCE_CROP:
    {
        const vout_display_cfg_t *cfg;
        const video_format_t *source;
        if (query == VOUT_DISPLAY_CHANGE_SOURCE_ASPECT
         || query == VOUT_DISPLAY_CHANGE_SOURCE_CROP)
        {
            source = (const video_format_t *)va_arg (ap, const video_format_t *);
            cfg = vd->cfg;
        }
        else
        {
            source = &vd->source;
            cfg = (const vout_display_cfg_t*)va_arg (ap, const vout_display_cfg_t *);
        }
        if (query == VOUT_DISPLAY_CHANGE_DISPLAY_SIZE && va_arg (ap, int))
        {
            vout_window_SetSize (p_sys->embed,
                                 cfg->display.width, cfg->display.height);
            return VLC_EGENERIC; /* Always fail. See x11.c for rationale. */
        }
        vout_display_place_t place;
        vout_display_PlacePicture (&place, source, cfg, false);
        p_sys->width  = place.width;
        p_sys->height = place.height;
        /* Move the picture within the window */
        const uint32_t values[] = { place.x, place.y,
                                    place.width, place.height, };
        xcb_configure_window (p_sys->conn, p_sys->window,
                              XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y
                            | XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT,
                              values);
        xcb_flush (p_sys->conn);
        return VLC_SUCCESS;
    }
    case VOUT_DISPLAY_CHANGE_WINDOW_STATE:
    {
        unsigned state = va_arg (ap, unsigned);
        return vout_window_SetState (p_sys->embed, state);
    }
    /* Hide the mouse. It will be send when
     * vout_display_t::info.b_hide_mouse is false */
    case VOUT_DISPLAY_HIDE_MOUSE:
        xcb_change_window_attributes (p_sys->conn, p_sys->embed->handle.xid,
                                  XCB_CW_CURSOR, &(uint32_t){ p_sys->cursor });
        xcb_flush (p_sys->conn);
        return VLC_SUCCESS;
    case VOUT_DISPLAY_RESET_PICTURES:
        assert(0);
    default:
        msg_Err (vd, "Unknown request in XCB vout display");
        return VLC_EGENERIC;
    }
}
