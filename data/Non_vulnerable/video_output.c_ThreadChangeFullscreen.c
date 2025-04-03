}
static void ThreadChangeFullscreen(vout_thread_t *vout, bool fullscreen)
{
    vout_window_t *window = vout->p->window.object;
    if (window != NULL)
        vout_window_SetFullScreen(window, fullscreen);
    else
    if (vout->p->display.vd != NULL)
        vout_display_SendEvent(vout->p->display.vd,
                               VOUT_DISPLAY_EVENT_FULLSCREEN, fullscreen);
}
