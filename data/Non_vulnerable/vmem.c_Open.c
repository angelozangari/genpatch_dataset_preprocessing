 *****************************************************************************/
static int Open(vlc_object_t *object)
{
    vout_display_t *vd = (vout_display_t *)object;
    vout_display_sys_t *sys = malloc(sizeof(*sys));
    if (unlikely(!sys))
        return VLC_ENOMEM;
    /* Get the callbacks */
    vlc_format_cb setup = var_InheritAddress(vd, "vmem-setup");
    sys->lock = var_InheritAddress(vd, "vmem-lock");
    if (sys->lock == NULL) {
        msg_Err(vd, "missing lock callback");
        free(sys);
        return VLC_EGENERIC;
    }
    sys->unlock = var_InheritAddress(vd, "vmem-unlock");
    sys->display = var_InheritAddress(vd, "vmem-display");
    sys->cleanup = var_InheritAddress(vd, "vmem-cleanup");
    sys->opaque = var_InheritAddress(vd, "vmem-data");
    sys->pool = NULL;
    /* Define the video format */
    video_format_t fmt;
    video_format_ApplyRotation(&fmt, &vd->fmt);
    if (setup != NULL) {
        char chroma[5];
        memcpy(chroma, &fmt.i_chroma, 4);
        chroma[4] = '\0';
        memset(sys->pitches, 0, sizeof(sys->pitches));
        memset(sys->lines, 0, sizeof(sys->lines));
        sys->count = setup(&sys->opaque, chroma, &fmt.i_width, &fmt.i_height,
                           sys->pitches, sys->lines);
        if (sys->count == 0) {
            msg_Err(vd, "video format setup failure (no pictures)");
            free(sys);
            return VLC_EGENERIC;
        }
        fmt.i_chroma = vlc_fourcc_GetCodecFromString(VIDEO_ES, chroma);
    } else {
        char *chroma = var_InheritString(vd, "vmem-chroma");
        fmt.i_chroma = vlc_fourcc_GetCodecFromString(VIDEO_ES, chroma);
        free(chroma);
        fmt.i_width  = var_InheritInteger(vd, "vmem-width");
        fmt.i_height = var_InheritInteger(vd, "vmem-height");
        sys->pitches[0] = var_InheritInteger(vd, "vmem-pitch");
        sys->lines[0] = fmt.i_height;
        for (size_t i = 1; i < PICTURE_PLANE_MAX; i++)
        {
            sys->pitches[i] = sys->pitches[0];
            sys->lines[i] = sys->lines[0];
        }
        sys->count = 1;
        sys->cleanup = NULL;
    }
    fmt.i_x_offset = fmt.i_y_offset = 0;
    fmt.i_visible_width = fmt.i_width;
    fmt.i_visible_height = fmt.i_height;
    if (!fmt.i_chroma) {
        msg_Err(vd, "vmem-chroma should be 4 characters long");
        free(sys);
        return VLC_EGENERIC;
    }
    /* Define the bitmasks */
    switch (fmt.i_chroma)
    {
    case VLC_CODEC_RGB15:
        fmt.i_rmask = 0x001f;
        fmt.i_gmask = 0x03e0;
        fmt.i_bmask = 0x7c00;
        break;
    case VLC_CODEC_RGB16:
        fmt.i_rmask = 0x001f;
        fmt.i_gmask = 0x07e0;
        fmt.i_bmask = 0xf800;
        break;
    case VLC_CODEC_RGB24:
    case VLC_CODEC_RGB32:
        fmt.i_rmask = 0xff0000;
        fmt.i_gmask = 0x00ff00;
        fmt.i_bmask = 0x0000ff;
        break;
    default:
        fmt.i_rmask = 0;
        fmt.i_gmask = 0;
        fmt.i_bmask = 0;
        break;
    }
    /* */
    vout_display_info_t info = vd->info;
    info.has_hide_mouse = true;
    /* */
    vd->sys     = sys;
    vd->fmt     = fmt;
    vd->info    = info;
    vd->pool    = Pool;
    vd->prepare = NULL;
    vd->display = Display;
    vd->control = Control;
    vd->manage  = NULL;
    /* */
    vout_display_SendEventFullscreen(vd, false);
    vout_display_SendEventDisplaySize(vd, fmt.i_width, fmt.i_height, false);
    return VLC_SUCCESS;
}
