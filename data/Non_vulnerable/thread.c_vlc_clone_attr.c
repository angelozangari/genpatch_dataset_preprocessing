}
static int vlc_clone_attr (vlc_thread_t *p_handle, bool detached,
                           void *(*entry) (void *), void *data, int priority)
{
    struct vlc_thread *th = malloc (sizeof (*th));
    if (unlikely(th == NULL))
        return ENOMEM;
    th->entry = entry;
    th->data = data;
    th->killable = false; /* not until vlc_entry() ! */
#if !VLC_WINSTORE_APP
    th->killed = false;
#else
    atomic_init(&th->killed, false);
#endif
    th->cleaners = NULL;
    /* When using the MSVCRT C library you have to use the _beginthreadex
     * function instead of CreateThread, otherwise you'll end up with
     * memory leaks and the signal functions not working (see Microsoft
     * Knowledge Base, article 104641) */
    uintptr_t h = _beginthreadex (NULL, 0, vlc_entry, th, 0, NULL);
    if (h == 0)
    {
        int err = errno;
        free (th);
        return err;
    }
    if (detached)
    {
        CloseHandle((HANDLE)h);
        th->id = NULL;
    }
    else
        th->id = (HANDLE)h;
    if (p_handle != NULL)
        *p_handle = th;
    if (priority)
        SetThreadPriority (th->id, priority);
    return 0;
}
