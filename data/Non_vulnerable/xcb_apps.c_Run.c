}
static void *Run (void *data)
{
    services_discovery_t *sd = data;
    services_discovery_sys_t *p_sys = sd->p_sys;
    xcb_connection_t *conn = p_sys->conn;
    int fd = xcb_get_file_descriptor (conn);
    if (fd == -1)
        return NULL;
    while (!xcb_connection_has_error (conn))
    {
        xcb_generic_event_t *ev;
        struct pollfd ufd = { .fd = fd, .events = POLLIN, };
        poll (&ufd, 1, -1);
        int canc = vlc_savecancel ();
        while ((ev = xcb_poll_for_event (conn)) != NULL)
        {
            if ((ev->response_type & 0x7F) == XCB_PROPERTY_NOTIFY)
            {
                 const xcb_property_notify_event_t *pn =
                     (xcb_property_notify_event_t *)ev;
                 if (pn->atom == p_sys->net_client_list)
                     UpdateApps (sd);
            }
            free (ev);
        }
        vlc_restorecancel (canc);
    }
    return NULL;
}
