}
static void ResetPictures (vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    if (!sys->pool)
        return;
    if (sys->seg_base != 0)
        for (unsigned i = 0; i < MAX_PICTURES; i++)
            xcb_shm_detach (sys->conn, sys->seg_base + i);
    picture_pool_Delete (sys->pool);
    sys->pool = NULL;
}
