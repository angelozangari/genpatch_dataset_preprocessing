 */
static int Open (vlc_object_t *obj)
{
    vout_display_t *vd = (vout_display_t *)obj;
    vout_display_sys_t *p_sys;
    if (!var_InheritBool (obj, "overlay"))
        return VLC_EGENERIC;
    else
    {   /* NOTE: Reject hardware surface formats. Blending would break. */
        const vlc_chroma_description_t *chroma =
            vlc_fourcc_GetChromaDescription(vd->source.i_chroma);
        if (chroma != NULL && chroma->plane_count == 0)
            return VLC_EGENERIC;
    }
    p_sys = malloc (sizeof (*p_sys));
    if (p_sys == NULL)
        return VLC_ENOMEM;
    vd->sys = p_sys;
    /* Connect to X */
    xcb_connection_t *conn;
    const xcb_screen_t *screen;
    uint16_t width, height;
    p_sys->embed = XCB_parent_Create (vd, &conn, &screen, &width, &height);
    if (p_sys->embed == NULL)
    {
        free (p_sys);
        return VLC_EGENERIC;
    }
    p_sys->conn = conn;
    p_sys->att = NULL;
    p_sys->pool = NULL;
    if (!CheckXVideo (vd, conn))
    {
        msg_Warn (vd, "Please enable XVideo 2.2 for faster video display");
        goto error;
    }
    p_sys->window = xcb_generate_id (conn);
    xcb_pixmap_t pixmap = xcb_generate_id (conn);
    /* Cache adaptors infos */
    xcb_xv_query_adaptors_reply_t *adaptors =
        xcb_xv_query_adaptors_reply (conn,
            xcb_xv_query_adaptors (conn, p_sys->embed->handle.xid), NULL);
    if (adaptors == NULL)
        goto error;
    int adaptor_selected = var_InheritInteger (obj, "xvideo-adaptor");
    int adaptor_current = -1;
    /* */
    video_format_t fmt;
    vout_display_place_t place;
    p_sys->port = 0;
    vout_display_PlacePicture (&place, &vd->source, vd->cfg, false);
    p_sys->width  = place.width;
    p_sys->height = place.height;
    xcb_xv_adaptor_info_iterator_t it;
    for (it = xcb_xv_query_adaptors_info_iterator (adaptors);
         it.rem > 0;
         xcb_xv_adaptor_info_next (&it))
    {
        const xcb_xv_adaptor_info_t *a = it.data;
        adaptor_current++;
        if (adaptor_selected != -1 && adaptor_selected != adaptor_current)
            continue;
        if (!(a->type & XCB_XV_TYPE_INPUT_MASK)
         || !(a->type & XCB_XV_TYPE_IMAGE_MASK))
            continue;
        /* Look for an image format */
        video_format_ApplyRotation(&fmt, &vd->fmt);
        free (p_sys->att);
        p_sys->att = FindFormat (obj, conn, &fmt, a, &p_sys->id);
        if (p_sys->att == NULL) /* No acceptable image formats */
            continue;
        /* Grab a port */
        for (unsigned i = 0; i < a->num_ports; i++)
        {
             xcb_xv_port_t port = a->base_id + i;
             xcb_xv_grab_port_reply_t *gr =
                 xcb_xv_grab_port_reply (conn,
                     xcb_xv_grab_port (conn, port, XCB_CURRENT_TIME), NULL);
             uint8_t result = gr ? gr->result : 0xff;
             free (gr);
             if (result == 0)
             {
                 p_sys->port = port;
                 goto grabbed_port;
             }
             msg_Dbg (vd, "cannot grab port %"PRIu32": Xv error %"PRIu8, port,
                      result);
        }
        continue; /* No usable port */
    grabbed_port:
        /* Found port - initialize selected format */
        msg_Dbg (vd, "using adaptor %.*s", (int)a->name_size,
                 xcb_xv_adaptor_info_name (a));
        msg_Dbg (vd, "using port %"PRIu32, p_sys->port);
        msg_Dbg (vd, "using image format 0x%"PRIx32, p_sys->id);
        /* Look for an X11 visual, create a window */
        xcb_xv_format_t *f = xcb_xv_adaptor_info_formats (a);
        for (uint_fast16_t i = a->num_formats; i > 0; i--, f++)
        {
            if (f->depth != screen->root_depth)
                continue; /* this would fail anyway */
            uint32_t mask =
                XCB_CW_BACK_PIXMAP |
                XCB_CW_BACK_PIXEL |
                XCB_CW_BORDER_PIXMAP |
                XCB_CW_BORDER_PIXEL |
                XCB_CW_EVENT_MASK |
                XCB_CW_COLORMAP;
            const uint32_t list[] = {
                /* XCB_CW_BACK_PIXMAP */
                pixmap,
                /* XCB_CW_BACK_PIXEL */
                screen->black_pixel,
                /* XCB_CW_BORDER_PIXMAP */
                pixmap,
                /* XCB_CW_BORDER_PIXEL */
                screen->black_pixel,
                /* XCB_CW_EVENT_MASK */
                XCB_EVENT_MASK_VISIBILITY_CHANGE,
                /* XCB_CW_COLORMAP */
                screen->default_colormap,
            };
            xcb_void_cookie_t c;
            xcb_create_pixmap (conn, f->depth, pixmap, screen->root, 1, 1);
            c = xcb_create_window_checked (conn, f->depth, p_sys->window,
                 p_sys->embed->handle.xid, place.x, place.y,
                 place.width, place.height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                 f->visual, mask, list);
            xcb_map_window (conn, p_sys->window);
            if (!XCB_error_Check (vd, conn, "cannot create X11 window", c))
            {
                msg_Dbg (vd, "using X11 visual ID 0x%"PRIx32
                         " (depth: %"PRIu8")", f->visual, f->depth);
                msg_Dbg (vd, "using X11 window 0x%08"PRIx32, p_sys->window);
                goto created_window;
            }
        }
        xcb_xv_ungrab_port (conn, p_sys->port, XCB_CURRENT_TIME);
        p_sys->port = 0;
        msg_Dbg (vd, "no usable X11 visual");
        continue; /* No workable XVideo format (visual/depth) */
    created_window:
        break;
    }
    free (adaptors);
    if (!p_sys->port)
    {
        msg_Err (vd, "no available XVideo adaptor");
        goto error;
    }
    /* Create graphic context */
    p_sys->gc = xcb_generate_id (conn);
    xcb_create_gc (conn, p_sys->gc, p_sys->window, 0, NULL);
    msg_Dbg (vd, "using X11 graphic context 0x%08"PRIx32, p_sys->gc);
    /* Disable color keying if applicable */
    {
        xcb_intern_atom_reply_t *r =
            xcb_intern_atom_reply (conn,
                xcb_intern_atom (conn, 1, 21, "XV_AUTOPAINT_COLORKEY"), NULL);
        if (r != NULL && r->atom != 0)
            xcb_xv_set_port_attribute(conn, p_sys->port, r->atom, 1);
        free(r);
    }
    /* Colour space */
    {
        xcb_intern_atom_reply_t *r =
            xcb_intern_atom_reply (conn,
                xcb_intern_atom (conn, 1, 13, "XV_ITURBT_709"), NULL);
        if (r != NULL && r->atom != 0)
            xcb_xv_set_port_attribute(conn, p_sys->port, r->atom,
                                      fmt.i_height > 576);
        free(r);
    }
    /* Create cursor */
    p_sys->cursor = XCB_cursor_Create (conn, screen);
    p_sys->shm = XCB_shm_Check (obj, conn);
    p_sys->visible = false;
    /* */
    vout_display_info_t info = vd->info;
    info.has_pictures_invalid = false;
    info.has_event_thread = true;
    /* Setup vout_display_t once everything is fine */
    p_sys->swap_uv = vlc_fourcc_AreUVPlanesSwapped (fmt.i_chroma,
                                                    vd->fmt.i_chroma);
    if (p_sys->swap_uv)
        fmt.i_chroma = vd->fmt.i_chroma;
    vd->fmt = fmt;
    vd->info = info;
    vd->pool = Pool;
    vd->prepare = NULL;
    vd->display = Display;
    vd->control = Control;
    vd->manage = Manage;
    /* */
    bool is_fullscreen = vd->cfg->is_fullscreen;
    if (is_fullscreen && vout_window_SetFullScreen (p_sys->embed, true))
        is_fullscreen = false;
    vout_display_SendEventFullscreen (vd, is_fullscreen);
    vout_display_SendEventDisplaySize (vd, width, height, is_fullscreen);
    return VLC_SUCCESS;
error:
    Close (obj);
    return VLC_EGENERIC;
}
