}
int vlc_set_priority (vlc_thread_t th, int priority)
{
    if (!SetThreadPriority (th->id, priority))
        return VLC_EGENERIC;
    return VLC_SUCCESS;
}
