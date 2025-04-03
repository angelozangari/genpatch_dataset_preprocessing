}
void vout_window_Delete(vout_window_t *window)
{
    if (!window)
        return;
    window_t *w = (window_t *)window;
    if (w->inhibit)
    {
        vlc_inhibit_Set (w->inhibit, VLC_INHIBIT_NONE);
        vlc_inhibit_Destroy (w->inhibit);
    }
    vlc_module_unload(w->module, vout_window_stop, window);
    vlc_object_release(window);
}
