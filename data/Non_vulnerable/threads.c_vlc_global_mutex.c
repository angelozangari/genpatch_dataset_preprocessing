/*** Global locks ***/
void vlc_global_mutex (unsigned n, bool acquire)
{
    static vlc_mutex_t locks[] = {
        VLC_STATIC_MUTEX,
        VLC_STATIC_MUTEX,
        VLC_STATIC_MUTEX,
        VLC_STATIC_MUTEX,
        VLC_STATIC_MUTEX,
        VLC_STATIC_MUTEX,
    };
    static_assert (VLC_MAX_MUTEX == (sizeof (locks) / sizeof (locks[0])),
                   "Wrong number of global mutexes");
    assert (n < (sizeof (locks) / sizeof (locks[0])));
    vlc_mutex_t *lock = locks + n;
    if (acquire)
        vlc_mutex_lock (lock);
    else
        vlc_mutex_unlock (lock);
}
