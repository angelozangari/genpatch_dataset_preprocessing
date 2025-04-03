}
static unsigned __stdcall vlc_entry (void *p)
{
    struct vlc_thread *th = p;
    vlc_threadvar_set (thread_key, th);
    th->killable = true;
    th->data = th->entry (th->data);
    vlc_thread_cleanup (th);
    return 0;
}
