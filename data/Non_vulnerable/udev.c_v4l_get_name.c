}
static char *v4l_get_name (struct udev_device *dev)
{
    const char *prd = udev_device_get_property_value (dev, "ID_V4L_PRODUCT");
    return prd ? strdup (prd) : NULL;
}
