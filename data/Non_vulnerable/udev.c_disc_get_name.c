}
static char *disc_get_name (struct udev_device *dev)
{
    char *name = NULL;
    struct udev_list_entry *list, *entry;
    list = udev_device_get_properties_list_entry (dev);
    if (unlikely(list == NULL))
        return NULL;
    const char *cat = NULL;
    udev_list_entry_foreach (entry, list)
    {
        const char *name = udev_list_entry_get_name (entry);
        if (strncmp (name, "ID_CDROM_MEDIA_", 15))
            continue;
        if (!atoi (udev_list_entry_get_value (entry)))
            continue;
        name += 15;
        if (!strncmp (name, "CD", 2))
            cat = N_("CD");
        else if (!strncmp (name, "DVD", 3))
            cat = N_("DVD");
        else if (!strncmp (name, "BD", 2))
            cat = N_("Blu-ray");
        else if (!strncmp (name, "HDDVD", 5))
            cat = N_("HD DVD");
        if (cat != NULL)
            break;
    }
    if (cat == NULL)
        cat = N_("Unknown type");
    char *label = decode_property (dev, "ID_FS_LABEL_ENC");
    if (label)
        if (asprintf(&name, "%s (%s)", label, vlc_gettext(cat)) < 0)
            name = NULL;
    free(label);
    return name;
}
