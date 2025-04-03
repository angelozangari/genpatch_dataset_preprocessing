}
static void context_state_cb (pa_context *ctx, void *userdata)
{
    pa_threaded_mainloop *mainloop = userdata;
    switch (pa_context_get_state(ctx))
    {
        case PA_CONTEXT_READY:
        case PA_CONTEXT_FAILED:
        case PA_CONTEXT_TERMINATED:
            pa_threaded_mainloop_signal (mainloop, 0);
        default:
            break;
    }
}
