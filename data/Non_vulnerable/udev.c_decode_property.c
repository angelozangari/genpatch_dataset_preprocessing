}
static char *decode_property (struct udev_device *dev, const char *name)
{
    return decode (udev_device_get_property_value (dev, name));
}
