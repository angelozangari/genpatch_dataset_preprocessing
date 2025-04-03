}
void vlc_cond_broadcast (vlc_cond_t *p_condvar)
{
    if (!p_condvar->clock)
        return;
    /* Wake all threads up (as the event HANDLE has manual reset) */
    SetEvent (p_condvar->handle);
}
