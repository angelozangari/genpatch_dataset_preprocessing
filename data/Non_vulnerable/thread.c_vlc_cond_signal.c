}
void vlc_cond_signal (vlc_cond_t *p_condvar)
{
    if (!p_condvar->clock)
        return;
    /* This is suboptimal but works. */
    vlc_cond_broadcast (p_condvar);
}
