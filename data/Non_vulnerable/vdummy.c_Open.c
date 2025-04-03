 *****************************************************************************/
static int Open(vlc_object_t *object,
                void (*display)(vout_display_t *, picture_t *, subpicture_t *))
{
    vout_display_t *vd = (vout_display_t *)object;
    vout_display_sys_t *sys;
    vd->sys = sys = calloc(1, sizeof(*sys));
    if (!sys)
        return VLC_EGENERIC;
    sys->pool = NULL;
    /* p_vd->info is not modified */
    char *chroma = var_InheritString(vd, "dummy-chroma");
    if (chroma) {
        vlc_fourcc_t fcc = vlc_fourcc_GetCodecFromString(VIDEO_ES, chroma);
        if (fcc != 0) {
            msg_Dbg(vd, "forcing chroma 0x%.8x (%4.4s)", fcc, (char*)&fcc);
            vd->fmt.i_chroma = fcc;
        }
        free(chroma);
    }
    vd->pool    = Pool;
    vd->prepare = NULL;
    vd->display = display;
    vd->control = Control;
    vd->manage  = NULL;
    return VLC_SUCCESS;
}
