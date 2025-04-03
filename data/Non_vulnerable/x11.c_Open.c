 */
static int Open (vlc_object_t *obj)
{
    vout_display_t *vd = (vout_display_t *)obj;
    vout_display_sys_t *sys = malloc (sizeof (*sys));
    if (unlikely(sys == NULL))
        return VLC_ENOMEM;
    vd->sys = sys;
    sys->pool = NULL;
    /* Get window, connect to X server */
    xcb_connection_t *conn;
    const xcb_screen_t *scr;
    uint16_t width, height;
    sys->embed = XCB_parent_Create (vd, &conn, &scr, &width, &height);
    if (sys->embed == NULL)
    {
        free (sys);
        return VLC_EGENERIC;
    }
    sys->conn = conn;
    const xcb_setup_t *setup = xcb_get_setup (conn);
    /* Determine our pixel format */
    video_format_t fmt_pic;
    xcb_visualid_t vid = 0;
    sys->depth = 0;
    for (const xcb_format_t *fmt = xcb_setup_pixmap_formats (setup),
             *end = fmt + xcb_setup_pixmap_formats_length (setup);
         fmt < end;
         fmt++)
    {
        if (fmt->depth <= sys->depth)
            continue; /* no better than earlier format */
        video_format_ApplyRotation(&fmt_pic, &vd->fmt);
        /* Check that the pixmap format is supported by VLC. */
        switch (fmt->depth)
        {
          case 32:
            if (fmt->bits_per_pixel != 32)
                continue;
            fmt_pic.i_chroma = VLC_CODEC_ARGB;
            break;
          case 24:
            if (fmt->bits_per_pixel == 32)
                fmt_pic.i_chroma = VLC_CODEC_RGB32;
            else if (fmt->bits_per_pixel == 24)
                fmt_pic.i_chroma = VLC_CODEC_RGB24;
            else
                continue;
            break;
          case 16:
            if (fmt->bits_per_pixel != 16)
                continue;
            fmt_pic.i_chroma = VLC_CODEC_RGB16;
            break;
          case 15:
            if (fmt->bits_per_pixel != 16)
                continue;
            fmt_pic.i_chroma = VLC_CODEC_RGB15;
            break;
          case 8:
            if (fmt->bits_per_pixel != 8)
                continue;
            fmt_pic.i_chroma = VLC_CODEC_RGB8;
            break;
          default:
            continue;
        }
        /* Byte sex is a non-issue for 8-bits. It can be worked around with
         * RGB masks for 24-bits. Too bad for 15-bits and 16-bits. */
        if (fmt->bits_per_pixel == 16 && setup->image_byte_order != ORDER)
            continue;
        /* Make sure the X server is sane */
        assert (fmt->bits_per_pixel > 0);
        if (unlikely(fmt->scanline_pad % fmt->bits_per_pixel))
            continue;
        /* Check that the selected screen supports this depth */
        const xcb_depth_t *d = FindDepth (scr, fmt->depth);
        if (d == NULL)
            continue;
        /* Find a visual type for the selected depth */
        const xcb_visualtype_t *vt = xcb_depth_visuals (d);
        /* First try True Color class */
        for (int i = xcb_depth_visuals_length (d); i > 0; i--)
        {
            if (vt->_class == XCB_VISUAL_CLASS_TRUE_COLOR)
            {
                fmt_pic.i_rmask = vt->red_mask;
                fmt_pic.i_gmask = vt->green_mask;
                fmt_pic.i_bmask = vt->blue_mask;
            found_visual:
                vid = vt->visual_id;
                msg_Dbg (vd, "using X11 visual ID 0x%"PRIx32, vid);
                sys->depth = fmt->depth;
                msg_Dbg (vd, " %"PRIu8" bits depth", sys->depth);
                msg_Dbg (vd, " %"PRIu8" bits per pixel", fmt->bits_per_pixel);
                msg_Dbg (vd, " %"PRIu8" bits line pad", fmt->scanline_pad);
                goto found_format;
            }
            vt++;
        }
        /* Then try Static Gray class */
        if (fmt->depth != 8)
            continue;
        vt = xcb_depth_visuals (d);
        for (int i = xcb_depth_visuals_length (d); i > 0 && !vid; i--)
        {
            if (vt->_class == XCB_VISUAL_CLASS_STATIC_GRAY)
            {
                fmt_pic.i_chroma = VLC_CODEC_GREY;
                goto found_visual;
            }
            vt++;
        }
    }
    msg_Err (obj, "no supported pixel format & visual");
    goto error;
found_format:;
    /* Create colormap (needed to select non-default visual) */
    xcb_colormap_t cmap;
    if (vid != scr->root_visual)
    {
        cmap = xcb_generate_id (conn);
        xcb_create_colormap (conn, XCB_COLORMAP_ALLOC_NONE,
                             cmap, scr->root, vid);
    }
    else
        cmap = scr->default_colormap;
    /* Create window */
    sys->window = xcb_generate_id (conn);
    sys->gc = xcb_generate_id (conn);
    xcb_pixmap_t pixmap = xcb_generate_id (conn);
    {
        const uint32_t mask =
            XCB_CW_BACK_PIXMAP |
            XCB_CW_BACK_PIXEL |
            XCB_CW_BORDER_PIXMAP |
            XCB_CW_BORDER_PIXEL |
            XCB_CW_EVENT_MASK |
            XCB_CW_COLORMAP;
        const uint32_t values[] = {
            /* XCB_CW_BACK_PIXMAP */
            pixmap,
            /* XCB_CW_BACK_PIXEL */
            scr->black_pixel,
            /* XCB_CW_BORDER_PIXMAP */
            pixmap,
            /* XCB_CW_BORDER_PIXEL */
            scr->black_pixel,
            /* XCB_CW_EVENT_MASK */
            XCB_EVENT_MASK_VISIBILITY_CHANGE,
            /* XCB_CW_COLORMAP */
            cmap,
        };
        xcb_void_cookie_t c;
        xcb_create_pixmap (conn, sys->depth, pixmap, scr->root, 1, 1);
        c = xcb_create_window_checked (conn, sys->depth, sys->window,
                                       sys->embed->handle.xid, 0, 0,
                                       width, height, 0,
                                       XCB_WINDOW_CLASS_INPUT_OUTPUT,
                                       vid, mask, values);
        xcb_map_window (conn, sys->window);
        /* Create graphic context (I wonder why the heck do we need this) */
        xcb_create_gc (conn, sys->gc, sys->window, 0, NULL);
        if (XCB_error_Check (vd, conn, "cannot create X11 window", c))
            goto error;
    }
    msg_Dbg (vd, "using X11 window %08"PRIx32, sys->window);
    msg_Dbg (vd, "using X11 graphic context %08"PRIx32, sys->gc);
    sys->cursor = XCB_cursor_Create (conn, scr);
    sys->visible = false;
    if (XCB_shm_Check (obj, conn))
    {
        sys->seg_base = xcb_generate_id (conn);
        for (unsigned i = 1; i < MAX_PICTURES; i++)
             xcb_generate_id (conn);
    }
    else
        sys->seg_base = 0;
    /* Setup vout_display_t once everything is fine */
    vd->info.has_pictures_invalid = true;
    vd->info.has_event_thread = true;
    vd->fmt = fmt_pic;
    vd->pool = Pool;
    vd->prepare = NULL;
    vd->display = Display;
    vd->control = Control;
    vd->manage = Manage;
    /* */
    bool is_fullscreen = vd->cfg->is_fullscreen;
    if (is_fullscreen && vout_window_SetFullScreen (sys->embed, true))
        is_fullscreen = false;
    vout_display_SendEventFullscreen (vd, is_fullscreen);
    vout_display_SendEventDisplaySize (vd, width, height, is_fullscreen);
    return VLC_SUCCESS;
error:
    Close (obj);
    return VLC_EGENERIC;
}
