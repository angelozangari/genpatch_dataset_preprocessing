}
void vlc_timer_destroy (vlc_timer_t timer)
{
    if (timer->handle != INVALID_HANDLE_VALUE)
        DeleteTimerQueueTimer (NULL, timer->handle, INVALID_HANDLE_VALUE);
    free (timer);
}
