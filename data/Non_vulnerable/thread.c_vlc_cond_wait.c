}
void vlc_cond_wait (vlc_cond_t *p_condvar, vlc_mutex_t *p_mutex)
{
    DWORD result;
    if (!p_condvar->clock)
    {   /* FIXME FIXME FIXME */
        msleep (50000);
        return;
    }
    do
    {
        vlc_testcancel ();
        vlc_mutex_unlock (p_mutex);
        result = vlc_WaitForSingleObject (p_condvar->handle, INFINITE);
        vlc_mutex_lock (p_mutex);
    }
    while (result == WAIT_IO_COMPLETION);
    ResetEvent (p_condvar->handle);
}
