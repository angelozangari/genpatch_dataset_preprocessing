}
static void Close(vlc_object_t *obj)
{
    intf_thread_t *intf = (intf_thread_t *)obj;
    intf_sys_t *sys = intf->p_sys;
    SendMessage(sys->window, WM_QUIT, 0, 0);
    vlc_join(sys->thread, NULL);
    free(sys);
}
