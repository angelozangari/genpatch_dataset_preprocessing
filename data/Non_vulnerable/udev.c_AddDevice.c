 */
static int AddDevice (services_discovery_t *sd, struct udev_device *dev)
{
    services_discovery_sys_t *p_sys = sd->p_sys;
    char *mrl = p_sys->subsys->get_mrl (dev);
    if (mrl == NULL)
        return 0; /* don't know if it was an error... */
    char *name = p_sys->subsys->get_name (dev);
    input_item_t *item = input_item_NewWithType (mrl, name ? name : mrl,
                                                 0, NULL, 0, -1,
                                                 p_sys->subsys->item_type);
    msg_Dbg (sd, "adding %s (%s)", mrl, name);
    free (name);
    free (mrl);
    if (item == NULL)
        return -1;
    struct device *d = malloc (sizeof (*d));
    if (d == NULL)
    {
        vlc_gc_decref (item);
        return -1;
    }
    d->devnum = udev_device_get_devnum (dev);
    d->item = item;
    d->sd = NULL;
    struct device **dp = tsearch (d, &p_sys->root, cmpdev);
    if (dp == NULL) /* Out-of-memory */
    {
        DestroyDevice (d);
        return -1;
    }
    if (*dp != d) /* Overwrite existing device */
    {
        DestroyDevice (*dp);
        *dp = d;
    }
    services_discovery_AddItem (sd, item, NULL);
    d->sd = sd;
    return 0;
}
