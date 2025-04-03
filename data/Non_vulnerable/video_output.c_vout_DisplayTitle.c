}
void vout_DisplayTitle(vout_thread_t *vout, const char *title)
{
    assert(title);
    vout_control_PushString(&vout->p->control, VOUT_CONTROL_OSD_TITLE, title);
}
