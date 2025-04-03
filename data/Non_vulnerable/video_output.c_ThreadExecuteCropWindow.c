}
static void ThreadExecuteCropWindow(vout_thread_t *vout,
                                    unsigned x, unsigned y,
                                    unsigned width, unsigned height)
{
    vout_SetDisplayCrop(vout->p->display.vd, 0, 0,
                        x, y, width, height);
}
