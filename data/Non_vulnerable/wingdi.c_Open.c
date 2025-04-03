/* */
static int Open(vlc_object_t *object)
{
    vout_display_t *vd = (vout_display_t *)object;
    vout_display_sys_t *sys;
    vd->sys = sys = calloc(1, sizeof(*sys));
    if (!sys)
        return VLC_ENOMEM;
    if (CommonInit(vd))
        goto error;
    /* */
    video_format_t fmt = vd->fmt;
    if (Init(vd, &fmt, fmt.i_width, fmt.i_height))
        goto error;
    vout_display_info_t info = vd->info;
    info.is_slow              = false;
    info.has_double_click     = true;
    info.has_hide_mouse       = false;
    info.has_pictures_invalid = true;
    /* */
    vd->fmt  = fmt;
    vd->info = info;
    vd->pool    = Pool;
    vd->prepare = NULL;
    vd->display = Display;
    vd->manage  = Manage;
    vd->control = Control;
    return VLC_SUCCESS;
error:
    Close(VLC_OBJECT(vd));
    return VLC_EGENERIC;
}
