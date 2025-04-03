}
static void ThreadInit(vout_thread_t *vout)
{
    vout->p->window.is_unused = true;
    vout->p->window.object    = NULL;
    vout->p->dead             = false;
    vout->p->is_late_dropped  = var_InheritBool(vout, "drop-late-frames");
    vout->p->pause.is_on      = false;
    vout->p->pause.date       = VLC_TS_INVALID;
    vout_chrono_Init(&vout->p->render, 5, 10000); /* Arbitrary initial time */
}
