 */
static void RemoveDevice (services_discovery_t *sd, struct udev_device *dev)
{
    services_discovery_sys_t *p_sys = sd->p_sys;
    dev_t num = udev_device_get_devnum (dev);
    struct device **dp = tfind (&(dev_t){ num }, &p_sys->root, cmpdev);
    if (dp == NULL)
        return;
    struct device *d = *dp;
    tdelete (d, &p_sys->root, cmpdev);
    DestroyDevice (d);
}
