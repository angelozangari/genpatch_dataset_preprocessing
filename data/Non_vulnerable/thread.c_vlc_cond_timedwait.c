}
int vlc_cond_timedwait (vlc_cond_t *p_condvar, vlc_mutex_t *p_mutex,
                        mtime_t deadline)
{
    DWORD result;
    do
    {
        vlc_testcancel ();
        mtime_t total;
        switch (p_condvar->clock)
        {
            case VLC_CLOCK_MONOTONIC:
                total = mdate();
                break;
            case VLC_CLOCK_REALTIME: /* FIXME? sub-second precision */
                total = CLOCK_FREQ * time (NULL);
                break;
            default:
                assert (!p_condvar->clock);
                /* FIXME FIXME FIXME */
                msleep (50000);
                return 0;
        }
        total = (deadline - total) / 1000;
        if( total < 0 )
            total = 0;
        DWORD delay = (total > 0x7fffffff) ? 0x7fffffff : total;
        vlc_mutex_unlock (p_mutex);
        result = vlc_WaitForSingleObject (p_condvar->handle, delay);
        vlc_mutex_lock (p_mutex);
    }
    while (result == WAIT_IO_COMPLETION);
    ResetEvent (p_condvar->handle);
    return (result == WAIT_OBJECT_0) ? 0 : ETIMEDOUT;
}
