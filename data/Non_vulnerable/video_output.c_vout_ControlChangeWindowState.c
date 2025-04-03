}
void vout_ControlChangeWindowState(vout_thread_t *vout, unsigned st)
{
    vout_control_PushInteger(&vout->p->control, VOUT_CONTROL_WINDOW_STATE, st);
}
