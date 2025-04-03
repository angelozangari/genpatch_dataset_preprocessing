};
static struct app *AddApp (services_discovery_t *sd, xcb_window_t xid)
{
    services_discovery_sys_t *p_sys = sd->p_sys;
    char *mrl, *name;
    if (asprintf (&mrl, "window://0x%"PRIx8, xid) == -1)
        return NULL;
    xcb_get_property_reply_t *r =
        xcb_get_property_reply (p_sys->conn,
            xcb_get_property (p_sys->conn, 0, xid, p_sys->net_wm_name, 0,
                              0, 1023 /* max size */), NULL);
    if (r != NULL)
    {
        name = strndup (xcb_get_property_value (r),
                        xcb_get_property_value_length (r));
        if (name != NULL)
            EnsureUTF8 (name); /* don't trust third party apps too much ;-) */
        free (r);
    }
    /* TODO: use WM_NAME (Latin-1) for very old apps */
    else
        name = NULL;
    input_item_t *item = input_item_NewWithType (mrl, name ? name : mrl,
                                                 0, NULL, 0, -1,
                                                 ITEM_TYPE_CARD /* FIXME */);
    free (mrl);
    free (name);
    if (item == NULL)
        return NULL;
    struct app *app = malloc (sizeof (*app));
    if (app == NULL)
    {
        vlc_gc_decref (item);
        return NULL;
    }
    app->xid = xid;
    app->item = item;
    app->owner = sd;
    services_discovery_AddItem (sd, item, _("Applications"));
    return app;
}
