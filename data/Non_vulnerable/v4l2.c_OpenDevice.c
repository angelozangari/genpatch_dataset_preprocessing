}
int OpenDevice (vlc_object_t *obj, const char *path, uint32_t *restrict caps)
{
    msg_Dbg (obj, "opening device '%s'", path);
    int rawfd = vlc_open (path, O_RDWR);
    if (rawfd == -1)
    {
        msg_Err (obj, "cannot open device '%s': %s", path,
                 vlc_strerror_c(errno));
        return -1;
    }
    int fd = v4l2_fd_open (rawfd, 0);
    if (fd == -1)
    {
        msg_Warn (obj, "cannot initialize user-space library: %s",
                  vlc_strerror_c(errno));
        /* fallback to direct kernel mode anyway */
        fd = rawfd;
    }
    /* Get device capabilites */
    struct v4l2_capability cap;
    if (v4l2_ioctl (fd, VIDIOC_QUERYCAP, &cap) < 0)
    {
        msg_Err (obj, "cannot get device capabilities: %s",
                 vlc_strerror_c(errno));
        v4l2_close (fd);
        return -1;
    }
    msg_Dbg (obj, "device %s using driver %s (version %u.%u.%u) on %s",
            cap.card, cap.driver, (cap.version >> 16) & 0xFF,
            (cap.version >> 8) & 0xFF, cap.version & 0xFF, cap.bus_info);
    if (cap.capabilities & V4L2_CAP_DEVICE_CAPS)
    {
        msg_Dbg (obj, " with capabilities 0x%08"PRIX32" "
                 "(overall 0x%08"PRIX32")", cap.device_caps, cap.capabilities);
        *caps = cap.device_caps;
    }
    else
    {
        msg_Dbg (obj, " with unknown capabilities  "
                 "(overall 0x%08"PRIX32")", cap.capabilities);
        *caps = cap.capabilities;
    }
    return fd;
}
