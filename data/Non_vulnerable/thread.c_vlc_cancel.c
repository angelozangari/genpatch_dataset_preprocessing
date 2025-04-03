#endif
void vlc_cancel (vlc_thread_t th)
{
#if !VLC_WINSTORE_APP
    QueueUserAPC (vlc_cancel_self, th->id, (uintptr_t)th);
#else
    atomic_store (&th->killed, true);
#endif
}
