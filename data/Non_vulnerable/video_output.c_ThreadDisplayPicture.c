}
static int ThreadDisplayPicture(vout_thread_t *vout, mtime_t *deadline)
{
    bool frame_by_frame = !deadline;
    bool paused = vout->p->pause.is_on;
    bool first = !vout->p->displayed.current;
    if (first)
        if (ThreadDisplayPreparePicture(vout, true, frame_by_frame)) /* FIXME not sure it is ok */
            return VLC_EGENERIC;
    if (!paused || frame_by_frame)
        while (!vout->p->displayed.next && !ThreadDisplayPreparePicture(vout, false, frame_by_frame))
            ;
    const mtime_t date = mdate();
    const mtime_t render_delay = vout_chrono_GetHigh(&vout->p->render) + VOUT_MWAIT_TOLERANCE;
    bool drop_next_frame = frame_by_frame;
    mtime_t date_next = VLC_TS_INVALID;
    if (!paused && vout->p->displayed.next) {
        date_next = vout->p->displayed.next->date - render_delay;
        if (date_next /* + 0 FIXME */ <= date)
            drop_next_frame = true;
    }
    /* FIXME/XXX we must redisplay the last decoded picture (because
     * of potential vout updated, or filters update or SPU update)
     * For now a high update period is needed but it could be removed
     * if and only if:
     * - vout module emits events from theselves.
     * - *and* SPU is modified to emit an event or a deadline when needed.
     *
     * So it will be done later.
     */
    bool refresh = false;
    mtime_t date_refresh = VLC_TS_INVALID;
    if (vout->p->displayed.date > VLC_TS_INVALID) {
        date_refresh = vout->p->displayed.date + VOUT_REDISPLAY_DELAY - render_delay;
        refresh = date_refresh <= date;
    }
    bool force_refresh = !drop_next_frame && refresh;
    if (!first && !refresh && !drop_next_frame) {
        if (!frame_by_frame) {
            if (date_refresh != VLC_TS_INVALID)
                *deadline = date_refresh;
            if (date_next != VLC_TS_INVALID && date_next < *deadline)
                *deadline = date_next;
        }
        return VLC_EGENERIC;
    }
    if (drop_next_frame) {
        picture_Release(vout->p->displayed.current);
        vout->p->displayed.current = vout->p->displayed.next;
        vout->p->displayed.next    = NULL;
    }
    if (!vout->p->displayed.current)
        return VLC_EGENERIC;
    /* display the picture immediately */
    bool is_forced = frame_by_frame || force_refresh || vout->p->displayed.current->b_force;
    int ret = ThreadDisplayRenderPicture(vout, is_forced);
    return force_refresh ? VLC_EGENERIC : ret;
}
