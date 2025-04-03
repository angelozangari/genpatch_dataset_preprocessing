}
void vlc_cond_init (vlc_cond_t *p_condvar)
{
    vlc_cond_init_common (p_condvar, VLC_CLOCK_MONOTONIC);
}
