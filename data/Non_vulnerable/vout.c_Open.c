}
static int Open(vlc_object_t *p_this)
{
    vout_display_t *vd = (vout_display_t *)p_this;
    vout_display_t *p_dec = vd;
    char ppsz_components[MAX_COMPONENTS_LIST_SIZE][OMX_MAX_STRINGNAME_SIZE];
    picture_t** pictures = NULL;
    OMX_PARAM_PORTDEFINITIONTYPE *def;
    static OMX_CALLBACKTYPE callbacks =
        { OmxEventHandler, OmxEmptyBufferDone, OmxFillBufferDone };
    if (InitOmxCore(p_this) != VLC_SUCCESS)
        return VLC_EGENERIC;
    int components = CreateComponentsList(p_this, "iv_renderer", ppsz_components);
    if (components <= 0) {
        DeinitOmxCore();
        return VLC_EGENERIC;
    }
    /* Allocate structure */
    vout_display_sys_t *p_sys = (struct vout_display_sys_t*) calloc(1, sizeof(*p_sys));
    if (!p_sys) {
        DeinitOmxCore();
        return VLC_ENOMEM;
    }
    vd->sys     = p_sys;
    strcpy(p_sys->psz_component, ppsz_components[0]);
    /* Load component */
    OMX_ERRORTYPE omx_error = pf_get_handle(&p_sys->omx_handle,
                                            p_sys->psz_component, vd, &callbacks);
    CHECK_ERROR(omx_error, "OMX_GetHandle(%s) failed (%x: %s)",
                p_sys->psz_component, omx_error, ErrorToString(omx_error));
    InitOmxEventQueue(&p_sys->event_queue);
    vlc_mutex_init (&p_sys->port.fifo.lock);
    vlc_cond_init (&p_sys->port.fifo.wait);
    p_sys->port.fifo.offset = offsetof(OMX_BUFFERHEADERTYPE, pOutputPortPrivate) / sizeof(void *);
    p_sys->port.fifo.pp_last = &p_sys->port.fifo.p_first;
    p_sys->port.b_direct = false;
    p_sys->port.b_flushed = true;
    OMX_PORT_PARAM_TYPE param;
    OMX_INIT_STRUCTURE(param);
    omx_error = OMX_GetParameter(p_sys->omx_handle, OMX_IndexParamVideoInit, &param);
    CHECK_ERROR(omx_error, "OMX_GetParameter(OMX_IndexParamVideoInit) failed (%x: %s)",
                omx_error, ErrorToString(omx_error));
    p_sys->port.i_port_index = param.nStartPortNumber;
    p_sys->port.b_valid = true;
    p_sys->port.omx_handle = p_sys->omx_handle;
    def = &p_sys->port.definition;
    OMX_INIT_STRUCTURE(*def);
    def->nPortIndex = p_sys->port.i_port_index;
    omx_error = OMX_GetParameter(p_sys->omx_handle, OMX_IndexParamPortDefinition, def);
    CHECK_ERROR(omx_error, "OMX_GetParameter(OMX_IndexParamPortDefinition) failed (%x: %s)",
                omx_error, ErrorToString(omx_error));
#define ALIGN(x, y) (((x) + ((y) - 1)) & ~((y) - 1))
    def->format.video.nFrameWidth = vd->fmt.i_width;
    def->format.video.nFrameHeight = vd->fmt.i_height;
    def->format.video.nStride = 0;
    def->format.video.nSliceHeight = 0;
    p_sys->port.definition.format.video.eColorFormat = OMX_COLOR_FormatYUV420PackedPlanar;
    if (!strcmp(p_sys->psz_component, "OMX.broadcom.video_render")) {
        def->format.video.nSliceHeight = ALIGN(def->format.video.nFrameHeight, 16);
    }
    omx_error = OMX_SetParameter(p_sys->omx_handle, OMX_IndexParamPortDefinition, &p_sys->port.definition);
    CHECK_ERROR(omx_error, "OMX_SetParameter(OMX_IndexParamPortDefinition) failed (%x: %s)",
                omx_error, ErrorToString(omx_error));
    OMX_GetParameter(p_sys->omx_handle, OMX_IndexParamPortDefinition, &p_sys->port.definition);
    if (def->format.video.nStride < (int) def->format.video.nFrameWidth)
        def->format.video.nStride = def->format.video.nFrameWidth;
    if (def->format.video.nSliceHeight < def->format.video.nFrameHeight)
        def->format.video.nSliceHeight = def->format.video.nFrameHeight;
    p_sys->port.pp_buffers =
            malloc(p_sys->port.definition.nBufferCountActual *
                   sizeof(OMX_BUFFERHEADERTYPE*));
    p_sys->port.i_buffers = p_sys->port.definition.nBufferCountActual;
    omx_error = OMX_SendCommand(p_sys->omx_handle, OMX_CommandStateSet, OMX_StateIdle, 0);
    CHECK_ERROR(omx_error, "OMX_CommandStateSet Idle failed (%x: %s)",
                omx_error, ErrorToString(omx_error));
    unsigned int i;
    for (i = 0; i < p_sys->port.i_buffers; i++) {
        omx_error = OMX_AllocateBuffer(p_sys->omx_handle, &p_sys->port.pp_buffers[i],
                                       p_sys->port.i_port_index, 0,
                                       p_sys->port.definition.nBufferSize);
        if (omx_error != OMX_ErrorNone)
            break;
        OMX_FIFO_PUT(&p_sys->port.fifo, p_sys->port.pp_buffers[i]);
    }
    if (omx_error != OMX_ErrorNone) {
        p_sys->port.i_buffers = i;
        for (i = 0; i < p_sys->port.i_buffers; i++)
            OMX_FreeBuffer(p_sys->omx_handle, p_sys->port.i_port_index, p_sys->port.pp_buffers[i]);
        msg_Err(vd, "OMX_AllocateBuffer failed (%x: %s)",
                omx_error, ErrorToString(omx_error));
        goto error;
    }
    omx_error = WaitForSpecificOmxEvent(&p_sys->event_queue, OMX_EventCmdComplete, 0, 0, 0);
    CHECK_ERROR(omx_error, "Wait for Idle failed (%x: %s)",
                omx_error, ErrorToString(omx_error));
    omx_error = OMX_SendCommand(p_sys->omx_handle, OMX_CommandStateSet,
                                OMX_StateExecuting, 0);
    CHECK_ERROR(omx_error, "OMX_CommandStateSet Executing failed (%x: %s)",
                omx_error, ErrorToString(omx_error));
    omx_error = WaitForSpecificOmxEvent(&p_sys->event_queue, OMX_EventCmdComplete, 0, 0, 0);
    CHECK_ERROR(omx_error, "Wait for Executing failed (%x: %s)",
                omx_error, ErrorToString(omx_error));
    if (!strcmp(p_sys->psz_component, "OMX.broadcom.video_render")) {
        OMX_CONFIG_DISPLAYREGIONTYPE config_display;
        OMX_INIT_STRUCTURE(config_display);
        config_display.nPortIndex = p_sys->port.i_port_index;
        config_display.set = OMX_DISPLAY_SET_SRC_RECT;
        config_display.src_rect.width = vd->cfg->display.width;
        config_display.src_rect.height = vd->cfg->display.height;
        OMX_SetConfig(p_sys->omx_handle, OMX_IndexConfigDisplayRegion, &config_display);
        config_display.set = OMX_DISPLAY_SET_FULLSCREEN;
        config_display.fullscreen = OMX_TRUE;
        OMX_SetConfig(p_sys->omx_handle, OMX_IndexConfigDisplayRegion, &config_display);
        UpdateDisplaySize(vd, vd->cfg);
    }
    /* Setup chroma */
    video_format_t fmt = vd->fmt;
    fmt.i_chroma = VLC_CODEC_I420;
    video_format_FixRgb(&fmt);
    /* Setup vout_display */
    vd->fmt     = fmt;
    vd->pool    = Pool;
    vd->display = Display;
    vd->control = Control;
    vd->prepare = NULL;
    vd->manage  = NULL;
    /* Create the associated picture */
    pictures = calloc(p_sys->port.i_buffers, sizeof(*pictures));
    if (!pictures)
        goto error;
    for (unsigned int i = 0; i < p_sys->port.i_buffers; i++) {
        picture_sys_t *picsys = malloc(sizeof(*picsys));
        if (unlikely(picsys == NULL))
            goto error;
        picsys->sys = p_sys;
        picture_resource_t resource = { .p_sys = picsys };
        picture_t *picture = picture_NewFromResource(&fmt, &resource);
        if (unlikely(picture == NULL))
        {
            free(picsys);
            goto error;
        }
        pictures[i] = picture;
    }
    /* Wrap it into a picture pool */
    picture_pool_configuration_t pool_cfg;
    memset(&pool_cfg, 0, sizeof(pool_cfg));
    pool_cfg.picture_count = p_sys->port.i_buffers;
    pool_cfg.picture       = pictures;
    pool_cfg.lock          = LockSurface;
    pool_cfg.unlock        = UnlockSurface;
    p_sys->pool = picture_pool_NewExtended(&pool_cfg);
    if (!p_sys->pool) {
        for (unsigned int i = 0; i < p_sys->port.i_buffers; i++)
            picture_Release(pictures[i]);
        goto error;
    }
    /* Fix initial state */
    vout_display_SendEventFullscreen(vd, true);
    free(pictures);
    return VLC_SUCCESS;
error:
    free(pictures);
    Close(p_this);
    return VLC_EGENERIC;
}
