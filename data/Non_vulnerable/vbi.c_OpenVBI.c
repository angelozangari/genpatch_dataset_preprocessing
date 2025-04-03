};
vlc_v4l2_vbi_t *OpenVBI (demux_t *demux, const char *psz_device)
{
    vlc_v4l2_vbi_t *vbi = malloc (sizeof (*vbi));
    if (unlikely(vbi == NULL))
        return NULL;
    int rawfd = vlc_open (psz_device, O_RDWR);
    if (rawfd == -1)
    {
        msg_Err (demux, "cannot open device '%s': %s", psz_device,
                 vlc_strerror_c(errno));
        goto err;
    }
    //Can put more in here. See osd.c in zvbi package.
    unsigned int services = VBI_SLICED_CAPTION_525;
    char *errstr = NULL;
    vbi->cap = vbi_capture_v4l2k_new (psz_device, rawfd,
                                      /* buffers */ 5,
                                      &services,
                                      /* strict */ 1,
                                      &errstr,
                                      /* verbose */ 1);
    if (vbi->cap == NULL)
    {
        msg_Err (demux, "cannot capture VBI data: %s", errstr);
        free (errstr);
        close (rawfd);
        goto err;
    }
    for (unsigned i = 0; i < VBI_NUM_CC_STREAMS; i++)
    {
        es_format_t fmt;
        es_format_Init (&fmt, SPU_ES, VLC_FOURCC('c', 'c', '1' + i, ' '));
        if (asprintf (&fmt.psz_description, "Closed captions %d", i + 1) >= 0)
        {
            msg_Dbg (demux, "new spu es %4.4s", (char *)&fmt.i_codec);
            vbi->es[i] = es_out_Add (demux->out, &fmt);
        }
    }
    /* Do a single read and throw away the results so that ZVBI calls
       the STREAMON ioctl() */
    GrabVBI(demux, vbi);
    return vbi;
err:
    free (vbi);
    return NULL;
}
