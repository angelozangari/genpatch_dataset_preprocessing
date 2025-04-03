static void AddDesktop(services_discovery_t *);
static int vlc_sd_probe_Open (vlc_object_t *obj)
{
    vlc_probe_t *probe = (vlc_probe_t *)obj;
    char *display = var_InheritString (obj, "x11-display");
    xcb_connection_t *conn = xcb_connect (display, NULL);
    free (display);
    if (xcb_connection_has_error (conn))
        return VLC_PROBE_CONTINUE;
    xcb_disconnect (conn);
    return vlc_sd_probe_Add (probe, "xcb_apps{longname=\"Screen capture\"}",
                             N_("Screen capture"), SD_CAT_DEVICES);
}
