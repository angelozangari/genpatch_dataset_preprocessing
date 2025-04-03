}
void vlc_restorecancel (int state)
{
    struct vlc_thread *th = vlc_threadvar_get (thread_key);
    assert (state == false || state == true);
    if (th == NULL)
        return; /* Main thread - cannot be cancelled anyway */
    assert (!th->killable);
    th->killable = state != 0;
}
