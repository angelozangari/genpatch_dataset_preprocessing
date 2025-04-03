}
static void Close(vlc_object_t *object)
{
    vout_display_t *vd = (vout_display_t *)object;
    vout_display_sys_t *sys = vd->sys;
    if (sys->cleanup)
        sys->cleanup(sys->opaque);
    picture_pool_Delete(sys->pool);
    free(sys);
}
