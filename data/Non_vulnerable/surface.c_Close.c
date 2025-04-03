}
static void Close(vlc_object_t *p_this)
{
    vout_display_t *vd = (vout_display_t *)p_this;
    vout_display_sys_t *sys = vd->sys;
    picture_pool_Delete(sys->pool);
    if (sys->window)
        sys->native_window.winRelease(sys->window);
    dlclose(sys->p_library);
    free(sys);
    vlc_mutex_unlock(&single_instance);
}
