}
void vout_ControlChangeZoom(vout_thread_t *vout, int num, int den)
{
    vout_control_PushPair(&vout->p->control, VOUT_CONTROL_ZOOM,
                          num, den);
}
