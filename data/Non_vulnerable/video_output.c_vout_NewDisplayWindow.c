}
vout_window_t * vout_NewDisplayWindow(vout_thread_t *vout, vout_display_t *vd,
                                      const vout_window_cfg_t *cfg)
{
    VLC_UNUSED(vd);
    vout_window_cfg_t cfg_override = *cfg;
    if (!var_InheritBool( vout, "embedded-video"))
        cfg_override.is_standalone = true;
    if (vout->p->window.is_unused && vout->p->window.object) {
        assert(!vout->p->splitter_name);
        if (!cfg_override.is_standalone == !vout->p->window.cfg.is_standalone &&
            cfg_override.type           == vout->p->window.cfg.type) {
            /* Reuse the stored window */
            msg_Dbg(vout, "Reusing previous vout window");
            vout_window_t *window = vout->p->window.object;
            if (cfg_override.width  != vout->p->window.cfg.width ||
                cfg_override.height != vout->p->window.cfg.height)
                vout_window_SetSize(window,
                                    cfg_override.width, cfg_override.height);
            vout->p->window.is_unused = false;
            vout->p->window.cfg       = cfg_override;
            return window;
        }
        vout_window_Delete(vout->p->window.object);
        vout->p->window.is_unused = true;
        vout->p->window.object    = NULL;
    }
    vout_window_t *window = vout_window_New(VLC_OBJECT(vout), "$window",
                                            &cfg_override);
    if (!window)
        return NULL;
    if (!vout->p->splitter_name) {
        vout->p->window.is_unused = false;
        vout->p->window.cfg       = cfg_override;
        vout->p->window.object    = window;
    }
    return window;
}
