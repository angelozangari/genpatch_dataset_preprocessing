}
static HRESULT Pause(aout_stream_t *s, bool paused)
{
    aout_stream_sys_t *sys = s->sys;
    HRESULT hr;
    if (paused)
        hr = IAudioClient_Stop(sys->client);
    else
        hr = IAudioClient_Start(sys->client);
    if (FAILED(hr))
        msg_Warn(s, "cannot %s stream (error 0x%lx)",
                 paused ? "stop" : "start", hr);
    return hr;
}
