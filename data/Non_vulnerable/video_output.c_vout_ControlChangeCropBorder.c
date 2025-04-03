}
void vout_ControlChangeCropBorder(vout_thread_t *vout,
                                  int left, int top, int right, int bottom)
{
    vout_control_cmd_t cmd;
    vout_control_cmd_Init(&cmd, VOUT_CONTROL_CROP_BORDER);
    cmd.u.border.left   = __MAX(left, 0);
    cmd.u.border.top    = __MAX(top, 0);
    cmd.u.border.right  = __MAX(right, 0);
    cmd.u.border.bottom = __MAX(bottom, 0);
    vout_control_Push(&vout->p->control, &cmd);
}
