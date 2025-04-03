}
static char *alsa_get_mrl (struct udev_device *dev)
{
    /* Determine media location */
    char *mrl;
    unsigned card, device;
    if (alsa_get_device (dev, &card, &device))
        return NULL;
    if (asprintf (&mrl, "alsa://plughw:%u,%u", card, device) == -1)
        mrl = NULL;
    return mrl;
}
