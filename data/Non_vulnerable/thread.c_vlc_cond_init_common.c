};
static void vlc_cond_init_common (vlc_cond_t *p_condvar, unsigned clock)
{
    /* Create a manual-reset event (manual reset is needed for broadcast). */
    p_condvar->handle = CreateEvent (NULL, TRUE, FALSE, NULL);
    if (!p_condvar->handle)
        abort();
    p_condvar->clock = clock;
}
