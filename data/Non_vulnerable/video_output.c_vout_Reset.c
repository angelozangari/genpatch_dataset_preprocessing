}
void vout_Reset(vout_thread_t *vout)
{
    vout_control_PushVoid(&vout->p->control, VOUT_CONTROL_RESET);
    vout_control_WaitEmpty(&vout->p->control);
}
