}
int vlc_clone (vlc_thread_t *p_handle, void *(*entry) (void *),
                void *data, int priority)
{
    return vlc_clone_attr (p_handle, false, entry, data, priority);
}
