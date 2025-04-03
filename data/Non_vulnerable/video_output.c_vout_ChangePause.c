/* */
void vout_ChangePause(vout_thread_t *vout, bool is_paused, mtime_t date)
{
    vout_control_cmd_t cmd;
    vout_control_cmd_Init(&cmd, VOUT_CONTROL_PAUSE);
    cmd.u.pause.is_on = is_paused;
    cmd.u.pause.date  = date;
    vout_control_Push(&vout->p->control, &cmd);
    vout_control_WaitEmpty(&vout->p->control);
}
