vlc_module_end ()
static int vlc_sd_probe_Open (vlc_object_t *obj)
{
    vlc_probe_t *probe = (vlc_probe_t *)obj;
    struct udev *udev = udev_new ();
    if (udev == NULL)
        return VLC_PROBE_CONTINUE;
    struct udev_monitor *mon = udev_monitor_new_from_netlink (udev, "udev");
    if (mon != NULL)
    {
        vlc_sd_probe_Add (probe, "v4l{longname=\"Video capture\"}",
                          N_("Video capture"), SD_CAT_DEVICES);
#ifdef HAVE_ALSA
        if (!module_exists ("pulselist"))
            vlc_sd_probe_Add (probe, "alsa{longname=\"Audio capture\"}",
                              N_("Audio capture"), SD_CAT_DEVICES);
#endif
        vlc_sd_probe_Add (probe, "disc{longname=\"Discs\"}", N_("Discs"),
                          SD_CAT_DEVICES);
        udev_monitor_unref (mon);
    }
    udev_unref (udev);
    return VLC_PROBE_CONTINUE;
}
