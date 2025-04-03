 */
void vlc_pa_disconnect (vlc_object_t *obj, pa_context *ctx,
                        pa_threaded_mainloop *mainloop)
{
    pa_threaded_mainloop_lock (mainloop);
    pa_context_disconnect (ctx);
    pa_context_set_event_callback (ctx, NULL, NULL);
    pa_context_set_state_callback (ctx, NULL, NULL);
    pa_context_unref (ctx);
    pa_threaded_mainloop_unlock (mainloop);
    pa_threaded_mainloop_stop (mainloop);
    pa_threaded_mainloop_free (mainloop);
    (void) obj;
}
