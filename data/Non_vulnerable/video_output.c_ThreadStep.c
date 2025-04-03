}
static void ThreadStep(vout_thread_t *vout, mtime_t *duration)
{
    *duration = 0;
    if (vout->p->step.last <= VLC_TS_INVALID)
        vout->p->step.last = vout->p->displayed.timestamp;
    if (ThreadDisplayPicture(vout, NULL))
        return;
    vout->p->step.timestamp = vout->p->displayed.timestamp;
    if (vout->p->step.last > VLC_TS_INVALID &&
        vout->p->step.timestamp > vout->p->step.last) {
        *duration = vout->p->step.timestamp - vout->p->step.last;
        vout->p->step.last = vout->p->step.timestamp;
        /* TODO advance subpicture by the duration ... */
    }
}
