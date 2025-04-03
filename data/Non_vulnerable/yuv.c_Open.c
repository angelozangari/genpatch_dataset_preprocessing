/* */
static int Open(vlc_object_t *object)
{
    vout_display_t *vd = (vout_display_t *)object;
    vout_display_sys_t *sys;
    /* Allocate instance and initialize some members */
    vd->sys = sys = malloc(sizeof(*sys));
    if (!sys)
        return VLC_ENOMEM;
    sys->is_first = false;
    sys->is_yuv4mpeg2 = var_InheritBool(vd, CFG_PREFIX "yuv4mpeg2");
    sys->pool = NULL;
    /* */
    char *psz_fcc = var_InheritString(vd, CFG_PREFIX "chroma");
    const vlc_fourcc_t requested_chroma = vlc_fourcc_GetCodecFromString(VIDEO_ES,
                                                                        psz_fcc);
    free(psz_fcc);
    const vlc_fourcc_t chroma = requested_chroma ? requested_chroma :
                                                   VLC_CODEC_I420;
    if (sys->is_yuv4mpeg2) {
        switch (chroma) {
        case VLC_CODEC_YV12:
        case VLC_CODEC_I420:
        case VLC_CODEC_J420:
            break;
        default:
            msg_Err(vd, "YUV4MPEG2 mode needs chroma YV12 not %4.4s as requested",
                    (char *)&chroma);
            free(sys);
            return VLC_EGENERIC;
        }
    }
    msg_Dbg(vd, "Using chroma %4.4s", (char *)&chroma);
    /* */
    char *name = var_InheritString(vd, CFG_PREFIX "file");
    if (!name) {
        msg_Err(vd, "Empty file name");
        free(sys);
        return VLC_EGENERIC;
    }
    sys->f = vlc_fopen(name, "wb");
    if (!sys->f) {
        msg_Err(vd, "Failed to open %s", name);
        free(name);
        free(sys);
        return VLC_EGENERIC;
    }
    msg_Dbg(vd, "Writing data to %s", name);
    free(name);
    /* */
    video_format_t fmt;
    video_format_ApplyRotation(&fmt, &vd->fmt);
    fmt.i_chroma = chroma;
    video_format_FixRgb(&fmt);
    /* */
    vout_display_info_t info = vd->info;
    info.has_hide_mouse = true;
    /* */
    vd->fmt     = fmt;
    vd->info    = info;
    vd->pool    = Pool;
    vd->prepare = NULL;
    vd->display = Display;
    vd->control = Control;
    vd->manage  = NULL;
    vout_display_SendEventFullscreen(vd, false);
    return VLC_SUCCESS;
}
