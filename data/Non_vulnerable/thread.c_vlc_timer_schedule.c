}
void vlc_timer_schedule (vlc_timer_t timer, bool absolute,
                         mtime_t value, mtime_t interval)
{
    if (timer->handle != INVALID_HANDLE_VALUE)
    {
        DeleteTimerQueueTimer (NULL, timer->handle, NULL);
        timer->handle = INVALID_HANDLE_VALUE;
    }
    if (value == 0)
        return; /* Disarm */
    if (absolute)
        value -= mdate ();
    value = (value + 999) / 1000;
    interval = (interval + 999) / 1000;
    if (!CreateTimerQueueTimer (&timer->handle, NULL, vlc_timer_do, timer,
                                value, interval, WT_EXECUTEDEFAULT))
        abort ();
}
