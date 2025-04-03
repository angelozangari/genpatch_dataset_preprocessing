 */
static int Open (vlc_object_t *obj)
{
    services_discovery_t *sd = (services_discovery_t *)obj;
    services_discovery_sys_t *p_sys = malloc (sizeof (*p_sys));
    if (p_sys == NULL)
        return VLC_ENOMEM;
    sd->p_sys = p_sys;
    /* Connect to X server */
    char *display = var_InheritString (obj, "x11-display");
    int snum;
    xcb_connection_t *conn = xcb_connect (display, &snum);
    free (display);
    if (xcb_connection_has_error (conn))
    {
        free (p_sys);
        return VLC_EGENERIC;
    }
    p_sys->conn = conn;
    /* Find configured screen */
    const xcb_setup_t *setup = xcb_get_setup (conn);
    const xcb_screen_t *scr = NULL;
    for (xcb_screen_iterator_t i = xcb_setup_roots_iterator (setup);
         i.rem > 0; xcb_screen_next (&i))
    {
        if (snum == 0)
        {
            scr = i.data;
            break;
        }
        snum--;
    }
    if (scr == NULL)
    {
        msg_Err (obj, "bad X11 screen number");
        goto error;
    }
    /* Add a permanent item for the entire desktop */
    AddDesktop (sd);
    p_sys->root_window = scr->root;
    xcb_change_window_attributes (conn, scr->root, XCB_CW_EVENT_MASK,
                               &(uint32_t) { XCB_EVENT_MASK_PROPERTY_CHANGE });
    /* TODO: check that _NET_CLIENT_LIST is in _NET_SUPPORTED
     * (and _NET_SUPPORTING_WM_CHECK) */
    xcb_intern_atom_reply_t *r;
    xcb_intern_atom_cookie_t ncl, nwn;
    ncl = xcb_intern_atom (conn, 1, strlen ("_NET_CLIENT_LIST"),
                          "_NET_CLIENT_LIST");
    nwn = xcb_intern_atom (conn, 0, strlen ("_NET_WM_NAME"), "_NET_WM_NAME");
    r = xcb_intern_atom_reply (conn, ncl, NULL);
    if (r == NULL || r->atom == 0)
    {
        dialog_Fatal (sd, _("Screen capture"),
            _("Your window manager does not provide a list of applications."));
        msg_Err (sd, "client list not supported (_NET_CLIENT_LIST absent)");
    }
    p_sys->net_client_list = r ? r->atom : 0;
    free (r);
    r = xcb_intern_atom_reply (conn, nwn, NULL);
    if (r != NULL)
    {
        p_sys->net_wm_name = r->atom;
        free (r);
    }
    p_sys->apps = NULL;
    UpdateApps (sd);
    if (vlc_clone (&p_sys->thread, Run, sd, VLC_THREAD_PRIORITY_LOW))
        goto error;
    return VLC_SUCCESS;
error:
    xcb_disconnect (p_sys->conn);
    free (p_sys);
    return VLC_EGENERIC;
}
