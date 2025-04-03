 *****************************************************************************/
void vout_ManageWrapper(vout_thread_t *vout)
{
    vout_thread_sys_t *sys = vout->p;
    vout_display_t *vd = sys->display.vd;
    bool reset_display_pool = vout_AreDisplayPicturesInvalid(vd);
    reset_display_pool |= vout_ManageDisplay(vd, !sys->display.use_dr || reset_display_pool);
    if (reset_display_pool) {
        sys->display.use_dr = !vout_IsDisplayFiltered(vd);
        NoDrInit(vout);
    }
}
