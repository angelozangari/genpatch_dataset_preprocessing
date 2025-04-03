}
static int Open(vlc_object_t *obj)
{
    intf_thread_t *intf = (intf_thread_t *)obj;
    intf_sys_t *sys = malloc(sizeof (*sys));
    if (unlikely(sys == NULL))
        return VLC_ENOMEM;
    intf->p_sys = sys;
    /* Run the helper thread */
    sys->ready = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (vlc_clone(&sys->thread, HelperThread, intf, VLC_THREAD_PRIORITY_LOW))
    {
        free(sys);
        msg_Err(intf, "one instance mode DISABLED "
                 "(IPC helper thread couldn't be created)");
        return VLC_ENOMEM;
    }
    WaitForSingleObject(sys->ready, INFINITE);
    CloseHandle(sys->ready);
    return VLC_SUCCESS;
}
