}
void vout_NextPicture(vout_thread_t *vout, mtime_t *duration)
{
    vout_control_cmd_t cmd;
    vout_control_cmd_Init(&cmd, VOUT_CONTROL_STEP);
    cmd.u.time_ptr = duration;
    vout_control_Push(&vout->p->control, &cmd);
    vout_control_WaitEmpty(&vout->p->control);
}
