 */
static void Close (vlc_object_t *obj)
{
    services_discovery_t *sd = (services_discovery_t *)obj;
    services_discovery_sys_t *p_sys = sd->p_sys;
    if (p_sys->monitor != NULL)
    {
        struct udev *udev = udev_monitor_get_udev (p_sys->monitor);
        vlc_cancel (p_sys->thread);
        vlc_join (p_sys->thread, NULL);
        udev_monitor_unref (p_sys->monitor);
        udev_unref (udev);
    }
    tdestroy (p_sys->root, DestroyDevice);
    free (p_sys);
}
