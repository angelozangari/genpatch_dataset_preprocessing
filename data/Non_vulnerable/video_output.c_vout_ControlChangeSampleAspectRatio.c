}
void vout_ControlChangeSampleAspectRatio(vout_thread_t *vout,
                                         unsigned num, unsigned den)
{
    vout_control_PushPair(&vout->p->control, VOUT_CONTROL_ASPECT_RATIO,
                          num, den);
}
