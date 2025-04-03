}
static void ThreadChangeWindowState(vout_thread_t *vout, unsigned state)
{
    vout_SetWindowState(vout->p->display.vd, state);
}
