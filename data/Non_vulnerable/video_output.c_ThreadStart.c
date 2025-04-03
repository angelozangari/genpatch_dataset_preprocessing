}
static int ThreadStart(vout_thread_t *vout, const vout_display_state_t *state)
{
    vlc_mouse_Init(&vout->p->mouse);
    vout->p->decoder_fifo = picture_fifo_New();
    vout->p->decoder_pool = NULL;
    vout->p->display_pool = NULL;
    vout->p->private_pool = NULL;
    vout->p->filter.configuration = NULL;
    video_format_Copy(&vout->p->filter.format, &vout->p->original);
    vout->p->filter.chain_static =
        filter_chain_New( vout, "video filter2", true,
                          VoutVideoFilterStaticAllocationSetup, NULL, vout);
    vout->p->filter.chain_interactive =
        filter_chain_New( vout, "video filter2", true,
                          VoutVideoFilterInteractiveAllocationSetup, NULL, vout);
    vout_display_state_t state_default;
    if (!state) {
        var_Create(vout, "video-wallpaper", VLC_VAR_BOOL|VLC_VAR_DOINHERIT);
        VoutGetDisplayCfg(vout, &state_default.cfg, vout->p->display.title);
        bool below = var_InheritBool(vout, "video-wallpaper");
        bool above = var_CreateGetBool(vout, "video-on-top");
        state_default.wm_state = below ? VOUT_WINDOW_STATE_BELOW
                               : above ? VOUT_WINDOW_STATE_ABOVE
                               : VOUT_WINDOW_STATE_NORMAL;
        state_default.sar.num = 0;
        state_default.sar.den = 0;
        state = &state_default;
    }
    if (vout_OpenWrapper(vout, vout->p->splitter_name, state))
        return VLC_EGENERIC;
    if (vout_InitWrapper(vout))
        return VLC_EGENERIC;
    assert(vout->p->decoder_pool);
    vout->p->displayed.current       = NULL;
    vout->p->displayed.next          = NULL;
    vout->p->displayed.decoded       = NULL;
    vout->p->displayed.date          = VLC_TS_INVALID;
    vout->p->displayed.timestamp     = VLC_TS_INVALID;
    vout->p->displayed.is_interlaced = false;
    vout->p->step.last               = VLC_TS_INVALID;
    vout->p->step.timestamp          = VLC_TS_INVALID;
    vout->p->spu_blend_chroma        = 0;
    vout->p->spu_blend               = NULL;
    video_format_Print(VLC_OBJECT(vout), "original format", &vout->p->original);
    return VLC_SUCCESS;
}
