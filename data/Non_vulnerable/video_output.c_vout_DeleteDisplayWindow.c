}
void vout_DeleteDisplayWindow(vout_thread_t *vout, vout_display_t *vd,
                              vout_window_t *window)
{
    VLC_UNUSED(vd);
    if (!vout->p->window.is_unused && vout->p->window.object == window) {
        vout->p->window.is_unused = true;
    } else if (vout->p->window.is_unused && vout->p->window.object && !window) {
        vout_window_Delete(vout->p->window.object);
        vout->p->window.is_unused = true;
        vout->p->window.object    = NULL;
    } else if (window) {
        vout_window_Delete(window);
    }
}
