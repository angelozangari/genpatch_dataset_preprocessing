}
vout_window_t *vout_window_New(vlc_object_t *obj,
                               const char *module,
                               const vout_window_cfg_t *cfg)
{
    window_t *w = vlc_custom_create(obj, sizeof(*w), "window");
    vout_window_t *window = &w->wnd;
    memset(&window->handle, 0, sizeof(window->handle));
    window->control = NULL;
    window->sys = NULL;
    window->type = cfg->type;
    const char *type;
    switch (cfg->type) {
#if defined(_WIN32) || defined(__OS2__)
    case VOUT_WINDOW_TYPE_HWND:
        type = "vout window hwnd";
        window->handle.hwnd = NULL;
        break;
#endif
#ifdef __APPLE__
    case VOUT_WINDOW_TYPE_NSOBJECT:
        type = "vout window nsobject";
        window->handle.nsobject = NULL;
        break;
#endif
    case VOUT_WINDOW_TYPE_XID:
        type = "vout window xid";
        window->handle.xid = 0;
        window->display.x11 = NULL;
        break;
    case VOUT_WINDOW_TYPE_ANDROID_NATIVE:
        type = "vout window anative";
        window->handle.anativewindow = NULL;
        break;
    default:
        assert(0);
    }
    w->module = vlc_module_load(window, type, module, module && *module,
                                vout_window_start, window, cfg);
    if (!w->module) {
        vlc_object_release(window);
        return NULL;
    }
    /* Hook for screensaver inhibition */
    if (var_InheritBool(obj, "disable-screensaver") &&
        cfg->type == VOUT_WINDOW_TYPE_XID) {
        w->inhibit = vlc_inhibit_Create(VLC_OBJECT (window));
        if (w->inhibit != NULL)
            vlc_inhibit_Set(w->inhibit, VLC_INHIBIT_VIDEO);
            /* FIXME: ^ wait for vout activation, pause */
    }
    else
        w->inhibit = NULL;
    return window;
}
