}
static void ThreadExecuteCropBorder(vout_thread_t *vout,
                                    unsigned left, unsigned top,
                                    unsigned right, unsigned bottom)
{
    msg_Err(vout, "ThreadExecuteCropBorder %d.%d %dx%d", left, top, right, bottom);
    vout_SetDisplayCrop(vout->p->display.vd, 0, 0,
                        left, top, -(int)right, -(int)bottom);
}
