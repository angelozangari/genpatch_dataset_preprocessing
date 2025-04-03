}
static HRESULT Flush(aout_stream_t *s)
{
    aout_stream_sys_t *sys = s->sys;
    HRESULT hr;
    IAudioClient_Stop(sys->client);
    hr = IAudioClient_Reset(sys->client);
    if (SUCCEEDED(hr))
    {
        msg_Dbg(s, "reset");
        sys->written = 0;
    }
    else
        msg_Warn(s, "cannot reset stream (error 0x%lx)", hr);
    return hr;
}
