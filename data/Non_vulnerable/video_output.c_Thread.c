 *****************************************************************************/
static void *Thread(void *object)
{
    vout_thread_t *vout = object;
    vout_thread_sys_t *sys = vout->p;
    vout_interlacing_support_t interlacing = {
        .is_interlaced = false,
        .date = mdate(),
    };
    mtime_t deadline = VLC_TS_INVALID;
    for (;;) {
        vout_control_cmd_t cmd;
        /* FIXME remove thoses ugly timeouts */
        while (!vout_control_Pop(&sys->control, &cmd, deadline, 100000))
            if (ThreadControl(vout, cmd))
                return NULL;
        vlc_mutex_lock(&sys->picture_lock);
        deadline = VLC_TS_INVALID;
        while (!ThreadDisplayPicture(vout, &deadline))
            ;
        const bool picture_interlaced = sys->displayed.is_interlaced;
        vlc_mutex_unlock(&sys->picture_lock);
        vout_SetInterlacingState(vout, &interlacing, picture_interlaced);
        vout_ManageWrapper(vout);
    }
}
