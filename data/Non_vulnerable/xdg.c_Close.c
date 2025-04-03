}
static void Close (vlc_object_t *obj)
{
    vlc_inhibit_t *ih = (vlc_inhibit_t *)obj;
    vlc_inhibit_sys_t *p_sys = ih->p_sys;
    vlc_timer_destroy (p_sys->timer);
    posix_spawnattr_destroy (&p_sys->attr);
    free (p_sys);
}
