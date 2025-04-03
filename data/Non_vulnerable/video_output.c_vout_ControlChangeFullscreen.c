/* vout_Control* are usable by anyone at anytime */
void vout_ControlChangeFullscreen(vout_thread_t *vout, bool fullscreen)
{
    vout_control_PushBool(&vout->p->control, VOUT_CONTROL_FULLSCREEN,
                          fullscreen);
}
