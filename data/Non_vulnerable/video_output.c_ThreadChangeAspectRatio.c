}
static void ThreadChangeAspectRatio(vout_thread_t *vout,
                                    unsigned num, unsigned den)
{
    vout_SetDisplayAspect(vout->p->display.vd, num, den);
}
