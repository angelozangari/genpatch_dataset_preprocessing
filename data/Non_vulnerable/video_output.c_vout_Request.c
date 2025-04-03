#undef vout_Request
vout_thread_t *vout_Request(vlc_object_t *object,
                              const vout_configuration_t *cfg)
{
    vout_thread_t *vout = cfg->vout;
    if (cfg->change_fmt && !cfg->fmt) {
        if (vout)
            vout_CloseAndRelease(vout);
        return NULL;
    }
    /* If a vout is provided, try reusing it */
    if (vout) {
        if (vout->p->input != cfg->input) {
            if (vout->p->input)
                spu_Attach(vout->p->spu, vout->p->input, false);
            vout->p->input = cfg->input;
            if (vout->p->input)
                spu_Attach(vout->p->spu, vout->p->input, true);
        }
        if (cfg->change_fmt) {
            vout_control_cmd_t cmd;
            vout_control_cmd_Init(&cmd, VOUT_CONTROL_REINIT);
            cmd.u.cfg = cfg;
            vout_control_Push(&vout->p->control, &cmd);
            vout_control_WaitEmpty(&vout->p->control);
        }
        if (!vout->p->dead) {
            msg_Dbg(object, "reusing provided vout");
            vout_IntfReinit(vout);
            return vout;
        }
        vout_CloseAndRelease(vout);
        msg_Warn(object, "cannot reuse provided vout");
    }
    return VoutCreate(object, cfg);
}
