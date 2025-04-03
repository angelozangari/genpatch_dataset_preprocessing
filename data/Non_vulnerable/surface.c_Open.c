}
static int Open(vlc_object_t *p_this)
{
    vout_display_t *vd = (vout_display_t *)p_this;
    video_format_t fmt;
    video_format_ApplyRotation(&fmt, &vd->fmt);
    if (fmt.i_chroma == VLC_CODEC_ANDROID_OPAQUE)
        return VLC_EGENERIC;
    /* */
    if (vlc_mutex_trylock(&single_instance) != 0) {
        msg_Err(vd, "Can't start more than one instance at a time");
        return VLC_EGENERIC;
    }
    /* Allocate structure */
    vout_display_sys_t *sys = (struct vout_display_sys_t*) calloc(1, sizeof(*sys));
    if (!sys) {
        vlc_mutex_unlock(&single_instance);
        return VLC_ENOMEM;
    }
    /* */
    sys->p_library = LoadNativeWindowAPI(&sys->native_window);
    sys->s_unlockAndPost = (Surface_unlockAndPost)sys->native_window.unlockAndPost;
    if (!sys->p_library)
        sys->p_library = InitLibrary(sys);
    if (!sys->p_library) {
        free(sys);
        msg_Err(vd, "Could not initialize libandroid.so/libui.so/libgui.so/libsurfaceflinger_client.so!");
        vlc_mutex_unlock(&single_instance);
        return VLC_EGENERIC;
    }
    /* Setup chroma */
    char *psz_fcc = var_InheritString(vd, CFG_PREFIX "chroma");
    if( psz_fcc ) {
        fmt.i_chroma = vlc_fourcc_GetCodecFromString(VIDEO_ES, psz_fcc);
        free(psz_fcc);
    } else
        fmt.i_chroma = VLC_CODEC_RGB32;
    switch(fmt.i_chroma) {
        case VLC_CODEC_YV12:
            /* avoid swscale usage by asking for I420 instead since the
             * vout already has code to swap the buffers */
            fmt.i_chroma = VLC_CODEC_I420;
        case VLC_CODEC_I420:
            break;
        case VLC_CODEC_RGB16:
            fmt.i_bmask = 0x0000001f;
            fmt.i_gmask = 0x000007e0;
            fmt.i_rmask = 0x0000f800;
            break;
        case VLC_CODEC_RGB32:
            fmt.i_rmask  = 0x000000ff;
            fmt.i_gmask  = 0x0000ff00;
            fmt.i_bmask  = 0x00ff0000;
            break;
        default:
            return VLC_EGENERIC;
    }
    video_format_FixRgb(&fmt);
    msg_Dbg(vd, "Pixel format %4.4s", (char*)&fmt.i_chroma);
    sys->fmt = fmt;
    /* Create the associated picture */
    picture_sys_t *picsys = malloc(sizeof(*picsys));
    if (unlikely(picsys == NULL))
        goto enomem;
    picsys->sys = sys;
    picture_resource_t resource = { .p_sys = picsys };
    picture_t *picture = picture_NewFromResource(&fmt, &resource);
    if (!picture) {
        free(picsys);
        goto enomem;
    }
    /* Wrap it into a picture pool */
    picture_pool_configuration_t pool_cfg;
    memset(&pool_cfg, 0, sizeof(pool_cfg));
    pool_cfg.picture_count = 1;
    pool_cfg.picture       = &picture;
    pool_cfg.lock          = AndroidLockSurface;
    pool_cfg.unlock        = AndroidUnlockSurface;
    sys->pool = picture_pool_NewExtended(&pool_cfg);
    if (!sys->pool) {
        picture_Release(picture);
        goto enomem;
    }
    /* Setup vout_display */
    vd->sys     = sys;
    vd->fmt     = fmt;
    vd->pool    = Pool;
    vd->display = Display;
    vd->control = Control;
    vd->prepare = NULL;
    vd->manage  = Manage;
    /* Fix initial state */
    vout_display_SendEventFullscreen(vd, false);
    sys->i_sar_num = vd->source.i_sar_num;
    sys->i_sar_den = vd->source.i_sar_den;
    return VLC_SUCCESS;
enomem:
    dlclose(sys->p_library);
    free(sys);
    vlc_mutex_unlock(&single_instance);
    return VLC_ENOMEM;
}
