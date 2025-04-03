#if VLC_WINSTORE_APP
static bool isCancelled(void)
{
    struct vlc_thread *th = vlc_threadvar_get (thread_key);
    if (th == NULL)
        return false; /* Main thread - cannot be cancelled anyway */
    return atomic_load(&th->killed);
}
