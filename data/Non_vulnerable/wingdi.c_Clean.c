}
static void Clean(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    if (sys->pool)
        picture_pool_Delete(sys->pool);
    sys->pool = NULL;
    if (sys->off_dc)
        DeleteDC(sys->off_dc);
    if (sys->off_bitmap)
        DeleteObject(sys->off_bitmap);
}
