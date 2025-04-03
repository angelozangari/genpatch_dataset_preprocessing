 */
void vlc_pa_rttime_free (pa_threaded_mainloop *mainloop, pa_time_event *e)
{
    pa_mainloop_api *api = pa_threaded_mainloop_get_api (mainloop);
    api->time_free (e);
}
