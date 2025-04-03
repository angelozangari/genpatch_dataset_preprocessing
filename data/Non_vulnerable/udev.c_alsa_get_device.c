#include <alsa/asoundlib.h>
static int alsa_get_device (struct udev_device *dev, unsigned *restrict pcard,
                            unsigned *restrict pdevice)
{
    const char *node = udev_device_get_devpath (dev);
    char type;
    node = strrchr (node, '/');
    if (node == NULL)
        return -1;
    if (sscanf (node, "/pcmC%uD%u%c", pcard, pdevice, &type) < 3)
        return -1;
    if (type != 'c')
        return -1;
    return 0;
}
