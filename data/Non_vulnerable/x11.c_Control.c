}
static int Control (vout_display_t *vd, int query, va_list ap)
{
    vout_display_sys_t *sys = vd->sys;
    switch (query)
    {
    case VOUT_DISPLAY_CHANGE_FULLSCREEN:
    {
        const vout_display_cfg_t *c = va_arg (ap, const vout_display_cfg_t *);
        return vout_window_SetFullScreen (sys->embed, c->is_fullscreen);
    }
    case VOUT_DISPLAY_CHANGE_DISPLAY_SIZE:
    {
        const vout_display_cfg_t *p_cfg =
            (const vout_display_cfg_t*)va_arg (ap, const vout_display_cfg_t *);
        const bool is_forced = (bool)va_arg (ap, int);
        if (is_forced)
        {   /* Changing the dimensions of the parent window takes place
             * asynchronously (in the X server). Also it might fail or result
             * in different dimensions than requested. Request the size change
             * and return a failure since the size is not (yet) changed.
             * If the change eventually succeeds, HandleParentStructure()
             * will trigger a non-forced display size change later. */
            vout_window_SetSize (sys->embed, p_cfg->display.width,
                                 p_cfg->display.height);
            return VLC_EGENERIC;
        }
        vout_display_place_t place;
        vout_display_PlacePicture (&place, &vd->source, p_cfg, false);
        if (place.width  != vd->fmt.i_visible_width ||
            place.height != vd->fmt.i_visible_height)
        {
            vout_display_SendEventPicturesInvalid (vd);
            return VLC_SUCCESS;
        }
        /* Move the picture within the window */
        const uint32_t values[] = { place.x, place.y };
        xcb_configure_window (sys->conn, sys->window,
                              XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y,
                              values);
        return VLC_SUCCESS;
    }
    case VOUT_DISPLAY_CHANGE_WINDOW_STATE:
    {
        unsigned state = va_arg (ap, unsigned);
        return vout_window_SetState (sys->embed, state);
    }
    case VOUT_DISPLAY_CHANGE_ZOOM:
    case VOUT_DISPLAY_CHANGE_DISPLAY_FILLED:
    case VOUT_DISPLAY_CHANGE_SOURCE_ASPECT:
    case VOUT_DISPLAY_CHANGE_SOURCE_CROP:
        /* I am not sure it is always necessary, but it is way simpler ... */
        vout_display_SendEventPicturesInvalid (vd);
        return VLC_SUCCESS;
    case VOUT_DISPLAY_RESET_PICTURES:
    {
        ResetPictures (vd);
        vout_display_place_t place;
        vout_display_PlacePicture (&place, &vd->source, vd->cfg, false);
        video_format_t src;
        video_format_ApplyRotation(&src, &vd->source);
        vd->fmt.i_width  = src.i_width  * place.width / src.i_visible_width;
        vd->fmt.i_height = src.i_height * place.height / src.i_visible_height;
        vd->fmt.i_visible_width  = place.width;
        vd->fmt.i_visible_height = place.height;
        vd->fmt.i_x_offset = src.i_x_offset * place.width / src.i_visible_width;
        vd->fmt.i_y_offset = src.i_y_offset * place.height / src.i_visible_height;
        return VLC_SUCCESS;
    }
    /* Hide the mouse. It will be send when
     * vout_display_t::info.b_hide_mouse is false */
    case VOUT_DISPLAY_HIDE_MOUSE:
        xcb_change_window_attributes (sys->conn, sys->embed->handle.xid,
                                  XCB_CW_CURSOR, &(uint32_t){ sys->cursor });
        xcb_flush (sys->conn);
        return VLC_SUCCESS;
    default:
        msg_Err (vd, "Unknown request in XCB vout display");
        return VLC_EGENERIC;
    }
}
