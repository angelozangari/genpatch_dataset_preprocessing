}
int vlc_savecancel (void)
{
    struct vlc_thread *th = vlc_threadvar_get (thread_key);
    if (th == NULL)
        return false; /* Main thread - cannot be cancelled anyway */
    int state = th->killable;
    th->killable = false;
    return state;
}
