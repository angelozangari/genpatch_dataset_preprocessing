}
void vout_PutSubpicture( vout_thread_t *vout, subpicture_t *subpic )
{
    vout_control_cmd_t cmd;
    vout_control_cmd_Init(&cmd, VOUT_CONTROL_SUBPICTURE);
    cmd.u.subpicture = subpic;
    vout_control_Push(&vout->p->control, &cmd);
}
