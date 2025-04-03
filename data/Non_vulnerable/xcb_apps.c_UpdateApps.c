} 
static void UpdateApps (services_discovery_t *sd)
{
    services_discovery_sys_t *p_sys = sd->p_sys;
    xcb_connection_t *conn = p_sys->conn;
    xcb_get_property_reply_t *r =
        xcb_get_property_reply (conn,
            xcb_get_property (conn, false, p_sys->root_window,
                              p_sys->net_client_list, XA_WINDOW, 0, 1024),
            NULL);
    if (r == NULL)
        return; /* FIXME: remove all entries */
    xcb_window_t *ent = xcb_get_property_value (r);
    int n = xcb_get_property_value_length (r) / 4;
    void *newnodes = NULL, *oldnodes = p_sys->apps;
    for (int i = 0; i < n; i++)
    {
        xcb_window_t id = *(ent++);
        struct app *app;
        struct app **pa = tfind (&id, &oldnodes, cmpapp);
        if (pa != NULL) /* existing entry */
        {
            app = *pa;
            tdelete (app, &oldnodes, cmpapp);
        }
        else /* new entry */
        {
            app = AddApp (sd, id);
            if (app == NULL)
                continue;
        }
        pa = tsearch (app, &newnodes, cmpapp);
        if (pa == NULL /* OOM */ || *pa != app /* buggy window manager */)
            DelApp (app);
    }
    free (r);
    /* Remove old nodes */
    tdestroy (oldnodes, DelApp);
    p_sys->apps = newnodes;
}
