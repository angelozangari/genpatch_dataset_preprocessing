#endif
static DWORD vlc_WaitForMultipleObjects (DWORD count, const HANDLE *handles,
                                         DWORD delay)
{
    DWORD ret;
    if (count == 0)
    {
#if VLC_WINSTORE_APP
        do {
            DWORD new_delay = 50;
            if (new_delay > delay)
                new_delay = delay;
            ret = SleepEx (new_delay, TRUE);
            if (delay != INFINITE)
                delay -= new_delay;
            if (isCancelled())
                ret = WAIT_IO_COMPLETION;
        } while (delay && ret == 0);
#else
        ret = SleepEx (delay, TRUE);
#endif
        if (ret == 0)
            ret = WAIT_TIMEOUT;
    }
    else {
#if VLC_WINSTORE_APP
        do {
            DWORD new_delay = 50;
            if (new_delay > delay)
                new_delay = delay;
            ret = WaitForMultipleObjectsEx (count, handles, FALSE, new_delay, TRUE);
            if (delay != INFINITE)
                delay -= new_delay;
            if (isCancelled())
                ret = WAIT_IO_COMPLETION;
        } while (delay && ret == WAIT_TIMEOUT);
#else
        ret = WaitForMultipleObjectsEx (count, handles, FALSE, delay, TRUE);
#endif
    }
    /* We do not abandon objects... this would be a bug */
    assert (ret < WAIT_ABANDONED_0 || WAIT_ABANDONED_0 + count - 1 < ret);
    if (unlikely(ret == WAIT_FAILED))
        abort (); /* We are screwed! */
    return ret;
}
