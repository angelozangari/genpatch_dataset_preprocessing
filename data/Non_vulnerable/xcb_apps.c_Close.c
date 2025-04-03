 */
static void Close (vlc_object_t *obj)
{
    services_discovery_t *sd = (services_discovery_t *)obj;
    services_discovery_sys_t *p_sys = sd->p_sys;
    vlc_cancel (p_sys->thread);
    vlc_join (p_sys->thread, NULL);
    xcb_disconnect (p_sys->conn);
    tdestroy (p_sys->apps, DelApp);
    free (p_sys);
}
