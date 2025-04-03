}
void vlc_testcancel (void)
{
    struct vlc_thread *th = vlc_threadvar_get (thread_key);
    if (th == NULL)
        return; /* Main thread - cannot be cancelled anyway */
    if (!th->killable)
        return;
#if !VLC_WINSTORE_APP
    if (likely(!th->killed))
        return;
#else
    if (!atomic_load(&th->killed))
        return;
#endif
    for (vlc_cleanup_t *p = th->cleaners; p != NULL; p = p->next)
        p->proc (p->data);
    th->data = NULL; /* TODO: special value? */
    vlc_thread_cleanup (th);
    _endthreadex(0);
}
