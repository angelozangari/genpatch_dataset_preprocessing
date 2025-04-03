}
static void ThreadExecuteCropRatio(vout_thread_t *vout,
                                   unsigned num, unsigned den)
{
    vout_SetDisplayCrop(vout->p->display.vd, num, den,
                        0, 0, 0, 0);
}
