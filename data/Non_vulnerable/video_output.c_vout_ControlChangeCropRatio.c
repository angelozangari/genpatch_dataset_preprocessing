}
void vout_ControlChangeCropRatio(vout_thread_t *vout,
                                 unsigned num, unsigned den)
{
    vout_control_PushPair(&vout->p->control, VOUT_CONTROL_CROP_RATIO,
                          num, den);
}
