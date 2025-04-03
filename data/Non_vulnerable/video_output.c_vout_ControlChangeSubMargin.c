}
void vout_ControlChangeSubMargin(vout_thread_t *vout, int margin)
{
    vout_control_PushInteger(&vout->p->control, VOUT_CONTROL_CHANGE_SUB_MARGIN,
                             margin);
}
