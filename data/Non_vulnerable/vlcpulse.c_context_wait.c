}
static bool context_wait (pa_context *ctx, pa_threaded_mainloop *mainloop)
{
    pa_context_state_t state;
    while ((state = pa_context_get_state (ctx)) != PA_CONTEXT_READY)
    {
        if (state == PA_CONTEXT_FAILED || state == PA_CONTEXT_TERMINATED)
            return -1;
        pa_threaded_mainloop_wait (mainloop);
    }
    return 0;
}
