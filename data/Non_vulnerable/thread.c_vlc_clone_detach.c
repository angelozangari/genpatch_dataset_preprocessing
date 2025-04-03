}
int vlc_clone_detach (vlc_thread_t *p_handle, void *(*entry) (void *),
                      void *data, int priority)
{
    vlc_thread_t th;
    if (p_handle == NULL)
        p_handle = &th;
    return vlc_clone_attr (p_handle, true, entry, data, priority);
}
