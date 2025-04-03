}
static void ThreadChangeZoom(vout_thread_t *vout, int num, int den)
{
    if (num * 10 < den) {
        num = den;
        den *= 10;
    } else if (num > den * 10) {
        num = den * 10;
    }
    vout_SetDisplayZoom(vout->p->display.vd, num, den);
}
