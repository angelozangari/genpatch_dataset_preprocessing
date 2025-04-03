}
static char *v4l_get_mrl (struct udev_device *dev)
{
    /* Determine media location */
    if (v4l_is_legacy (dev) || !v4l_can_capture (dev))
        return NULL;
    const char *node = udev_device_get_devnode (dev);
    char *mrl;
    if (asprintf (&mrl, "v4l2://%s", node) == -1)
        mrl = NULL;
    return mrl;
}
