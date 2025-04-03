 */
static int Open (vlc_object_t *obj, const struct subsys *subsys)
{
    services_discovery_t *sd = (services_discovery_t *)obj;
    services_discovery_sys_t *p_sys = malloc (sizeof (*p_sys));
    if (p_sys == NULL)
        return VLC_ENOMEM;
    sd->p_sys = p_sys;
    p_sys->subsys = subsys;
    p_sys->root = NULL;
    struct udev_monitor *mon = NULL;
    struct udev *udev = udev_new ();
    if (udev == NULL)
        goto error;
    mon = udev_monitor_new_from_netlink (udev, "udev");
    if (mon == NULL
     || udev_monitor_filter_add_match_subsystem_devtype (mon, subsys->name,
                                                         NULL))
        goto error;
    p_sys->monitor = mon;
    /* Enumerate existing devices */
    struct udev_enumerate *devenum = udev_enumerate_new (udev);
    if (devenum == NULL)
        goto error;
    if (udev_enumerate_add_match_subsystem (devenum, subsys->name))
    {
        udev_enumerate_unref (devenum);
        goto error;
    }
    udev_monitor_enable_receiving (mon);
    /* Note that we enumerate _after_ monitoring is enabled so that we do not
     * loose device events occuring while we are enumerating. We could still
     * loose events if the Netlink socket receive buffer overflows. */
    udev_enumerate_scan_devices (devenum);
    struct udev_list_entry *devlist = udev_enumerate_get_list_entry (devenum);
    struct udev_list_entry *deventry;
    udev_list_entry_foreach (deventry, devlist)
    {
        const char *path = udev_list_entry_get_name (deventry);
        struct udev_device *dev = udev_device_new_from_syspath (udev, path);
        AddDevice (sd, dev);
        udev_device_unref (dev);
    }
    udev_enumerate_unref (devenum);
    if (vlc_clone (&p_sys->thread, Run, sd, VLC_THREAD_PRIORITY_LOW))
    {   /* Fallback without thread */
        udev_monitor_unref (mon);
        udev_unref (udev);
        p_sys->monitor = NULL;
    }
    return VLC_SUCCESS;
error:
    if (mon != NULL)
        udev_monitor_unref (mon);
    if (udev != NULL)
        udev_unref (udev);
    free (p_sys);
    return VLC_EGENERIC;
}
