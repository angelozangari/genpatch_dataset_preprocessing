}
static void Stop(aout_stream_t *s)
{
    aout_stream_sys_t *sys = s->sys;
    IAudioClient_Stop(sys->client); /* should not be needed */
    IAudioClient_Release(sys->client);
    free(sys);
}
