}
static int ThreadReinit(vout_thread_t *vout,
                        const vout_configuration_t *cfg)
{
    video_format_t original;
    if (VoutValidateFormat(&original, cfg->fmt)) {
        ThreadStop(vout, NULL);
        ThreadClean(vout);
        return VLC_EGENERIC;
    }
    /* We ignore crop/ar changes at this point, they are dynamically supported */
    VideoFormatCopyCropAr(&vout->p->original, &original);
    if (video_format_IsSimilar(&original, &vout->p->original)) {
        if (cfg->dpb_size <= vout->p->dpb_size)
            return VLC_SUCCESS;
        msg_Warn(vout, "DPB need to be increased");
    }
    vout_display_state_t state;
    memset(&state, 0, sizeof(state));
    ThreadStop(vout, &state);
    if (!state.cfg.is_fullscreen) {
        state.cfg.display.width  = 0;
        state.cfg.display.height = 0;
    }
    state.sar.num = 0;
    state.sar.den = 0;
    /* FIXME current vout "variables" are not in sync here anymore
     * and I am not sure what to do */
    if (state.cfg.display.sar.num <= 0 || state.cfg.display.sar.den <= 0) {
        state.cfg.display.sar.num = 1;
        state.cfg.display.sar.den = 1;
    }
    if (state.cfg.zoom.num <= 0 || state.cfg.zoom.den <= 0) {
        state.cfg.zoom.num = 1;
        state.cfg.zoom.den = 1;
    }
    vout->p->original = original;
    vout->p->dpb_size = cfg->dpb_size;
    if (ThreadStart(vout, &state)) {
        ThreadClean(vout);
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
