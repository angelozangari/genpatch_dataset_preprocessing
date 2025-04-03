}
static bool v4l_can_capture (struct udev_device *dev)
{
    const char *caps;
    caps = udev_device_get_property_value (dev, "ID_V4L_CAPABILITIES");
    return (caps != NULL) && (strstr (caps, ":capture:") != NULL);
}
