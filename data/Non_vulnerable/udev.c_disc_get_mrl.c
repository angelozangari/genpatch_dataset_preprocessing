/*** Discs support ***/
static char *disc_get_mrl (struct udev_device *dev)
{
    const char *node = udev_device_get_devnode (dev);
    const char *val;
    val = udev_device_get_property_value (dev, "ID_CDROM");
    if (val == NULL)
        return NULL; /* Ignore non-optical block devices */
    val = udev_device_get_property_value (dev, "ID_CDROM_MEDIA_STATE");
    if (val == NULL)
    {   /* Force probing of the disc in the drive if any. */
        int fd = open (node, O_RDONLY|O_CLOEXEC);
        if (fd != -1)
            close (fd);
        return NULL;
    }
    if (!strcmp (val, "blank"))
        return NULL; /* ignore empty drives and virgin recordable discs */
    const char *scheme = NULL;
    val = udev_device_get_property_value (dev,
                                          "ID_CDROM_MEDIA_TRACK_COUNT_AUDIO");
    if (val && atoi (val))
        scheme = "cdda"; /* Audio CD rather than file system */
    val = udev_device_get_property_value (dev, "ID_CDROM_MEDIA_DVD");
    if (val && atoi (val))
        scheme = "dvd";
    val = udev_device_get_property_value (dev, "ID_CDROM_MEDIA_BD");
    if (val && atoi (val))
        scheme = "bluray";
#ifdef LOL
    val = udev_device_get_property_value (dev, "ID_CDROM_MEDIA_HDDVD");
    if (val && atoi (val))
        scheme = "hddvd";
#endif
    /* We didn't get any property that could tell we have optical disc
       that we can play */
    if (scheme == NULL)
        return NULL;
    return vlc_path2uri (node, scheme);
}
