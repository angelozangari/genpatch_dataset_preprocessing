}
void vout_ControlChangeCropWindow(vout_thread_t *vout,
                                  int x, int y, int width, int height)
{
    vout_control_cmd_t cmd;
    vout_control_cmd_Init(&cmd, VOUT_CONTROL_CROP_WINDOW);
    cmd.u.window.x      = __MAX(x, 0);
    cmd.u.window.y      = __MAX(y, 0);
    cmd.u.window.width  = __MAX(width, 0);
    cmd.u.window.height = __MAX(height, 0);
    vout_control_Push(&vout->p->control, &cmd);
}
