}
static int Open (vlc_object_t *obj)
{
    vlc_inhibit_t *ih = (vlc_inhibit_t *)obj;
    vlc_inhibit_sys_t *p_sys = malloc (sizeof (*p_sys));
    if (p_sys == NULL)
        return VLC_ENOMEM;
    posix_spawnattr_init (&p_sys->attr);
    /* Reset signal handlers to default and clear mask in the child process */
    {
        sigset_t set;
        sigemptyset (&set);
        posix_spawnattr_setsigmask (&p_sys->attr, &set);
        sigaddset (&set, SIGPIPE);
        posix_spawnattr_setsigdefault (&p_sys->attr, &set);
        posix_spawnattr_setflags (&p_sys->attr, POSIX_SPAWN_SETSIGDEF
                                              | POSIX_SPAWN_SETSIGMASK);
    }
    ih->p_sys = p_sys;
    if (vlc_timer_create (&p_sys->timer, Timer, ih))
    {
        posix_spawnattr_destroy (&p_sys->attr);
        free (p_sys);
        return VLC_ENOMEM;
    }
    ih->inhibit = Inhibit;
    return VLC_SUCCESS;
}
