}
static void *Run (void *data)
{
    services_discovery_t *sd = data;
    services_discovery_sys_t *p_sys = sd->p_sys;
    struct udev_monitor *mon = p_sys->monitor;
    int fd = udev_monitor_get_fd (mon);
    struct pollfd ufd = { .fd = fd, .events = POLLIN, };
    for (;;)
    {
        while (poll (&ufd, 1, -1) == -1)
            if (errno != EINTR)
                break;
        int canc = vlc_savecancel ();
        struct udev_device *dev = udev_monitor_receive_device (mon);
        if (dev == NULL)
            continue;
        const char *action = udev_device_get_action (dev);
        if (!strcmp (action, "add"))
            AddDevice (sd, dev);
        else if (!strcmp (action, "remove"))
            RemoveDevice (sd, dev);
        else if (!strcmp (action, "change"))
        {
            RemoveDevice (sd, dev);
            AddDevice (sd, dev);
        }
        udev_device_unref (dev);
        vlc_restorecancel (canc);
    }
    return NULL;
}
