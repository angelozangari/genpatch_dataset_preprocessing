#define DISPLAY_PICTURE_COUNT (1)
static void NoDrInit(vout_thread_t *vout)
{
    vout_thread_sys_t *sys = vout->p;
    if (sys->display.use_dr)
        sys->display_pool = vout_display_Pool(sys->display.vd, 3);
    else
        sys->display_pool = NULL;
}
