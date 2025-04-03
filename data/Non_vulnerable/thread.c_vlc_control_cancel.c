}
void vlc_control_cancel (int cmd, ...)
{
    /* NOTE: This function only modifies thread-specific data, so there is no
     * need to lock anything. */
    va_list ap;
    struct vlc_thread *th = vlc_threadvar_get (thread_key);
    if (th == NULL)
        return; /* Main thread - cannot be cancelled anyway */
    va_start (ap, cmd);
    switch (cmd)
    {
        case VLC_CLEANUP_PUSH:
        {
            /* cleaner is a pointer to the caller stack, no need to allocate
             * and copy anything. As a nice side effect, this cannot fail. */
            vlc_cleanup_t *cleaner = va_arg (ap, vlc_cleanup_t *);
            cleaner->next = th->cleaners;
            th->cleaners = cleaner;
            break;
        }
        case VLC_CLEANUP_POP:
        {
            th->cleaners = th->cleaners->next;
            break;
        }
    }
    va_end (ap);
}
