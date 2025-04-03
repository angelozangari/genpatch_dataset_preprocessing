}
static vout_thread_t *VoutCreate(vlc_object_t *object,
                                 const vout_configuration_t *cfg)
{
    video_format_t original;
    if (VoutValidateFormat(&original, cfg->fmt))
        return NULL;
    /* Allocate descriptor */
    vout_thread_t *vout = vlc_custom_create(object,
                                            sizeof(*vout) + sizeof(*vout->p),
                                            "video output");
    if (!vout) {
        video_format_Clean(&original);
        return NULL;
    }
    /* */
    vout->p = (vout_thread_sys_t*)&vout[1];
    vout->p->original = original;
    vout->p->dpb_size = cfg->dpb_size;
    vout_control_Init(&vout->p->control);
    vout_control_PushVoid(&vout->p->control, VOUT_CONTROL_INIT);
    vout_statistic_Init(&vout->p->statistic);
    vout_snapshot_Init(&vout->p->snapshot);
    /* Initialize locks */
    vlc_mutex_init(&vout->p->picture_lock);
    vlc_mutex_init(&vout->p->filter.lock);
    vlc_mutex_init(&vout->p->spu_lock);
    /* Initialize subpicture unit */
    vout->p->spu = spu_Create(vout);
    /* Take care of some "interface/control" related initialisations */
    vout_IntfInit(vout);
    vout->p->title.show     = var_InheritBool(vout, "video-title-show");
    vout->p->title.timeout  = var_InheritInteger(vout, "video-title-timeout");
    vout->p->title.position = var_InheritInteger(vout, "video-title-position");
    /* Get splitter name if present */
    char *splitter_name = var_InheritString(vout, "video-splitter");
    if (splitter_name && *splitter_name) {
        vout->p->splitter_name = splitter_name;
    } else {
        free(splitter_name);
    }
    /* */
    vout_InitInterlacingSupport(vout, vout->p->displayed.is_interlaced);
    /* */
    vlc_object_set_destructor(vout, VoutDestructor);
    /* */
    if (vlc_clone(&vout->p->thread, Thread, vout,
                  VLC_THREAD_PRIORITY_OUTPUT)) {
        spu_Destroy(vout->p->spu);
        vlc_object_release(vout);
        return NULL;
    }
    vout_control_WaitEmpty(&vout->p->control);
    if (vout->p->dead) {
        msg_Err(vout, "video output creation failed");
        vout_CloseAndRelease(vout);
        return NULL;
    }
    vout->p->input = cfg->input;
    if (vout->p->input)
        spu_Attach(vout->p->spu, vout->p->input, true);
    return vout;
}
