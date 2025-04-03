/*** Video4Linux support ***/
static bool v4l_is_legacy (struct udev_device *dev)
{
    const char *version;
    version = udev_device_get_property_value (dev, "ID_V4L_VERSION");
    return (version != NULL) && !strcmp (version, "1");
}
