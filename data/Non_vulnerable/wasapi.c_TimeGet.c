/*** VLC audio output callbacks ***/
static HRESULT TimeGet(aout_stream_t *s, mtime_t *restrict delay)
{
    aout_stream_sys_t *sys = s->sys;
    void *pv;
    UINT64 pos, qpcpos, freq;
    HRESULT hr;
    hr = IAudioClient_GetService(sys->client, &IID_IAudioClock, &pv);
    if (FAILED(hr))
    {
        msg_Err(s, "cannot get clock (error 0x%lx)", hr);
        return hr;
    }
    IAudioClock *clock = pv;
    hr = IAudioClock_GetPosition(clock, &pos, &qpcpos);
    if (SUCCEEDED(hr))
        hr = IAudioClock_GetFrequency(clock, &freq);
    IAudioClock_Release(clock);
    if (FAILED(hr))
    {
        msg_Err(s, "cannot get position (error 0x%lx)", hr);
        return hr;
    }
    lldiv_t w = lldiv(sys->written, sys->rate);
    lldiv_t r = lldiv(pos, freq);
    static_assert((10000000 % CLOCK_FREQ) == 0, "Frequency conversion broken");
    *delay = ((w.quot - r.quot) * CLOCK_FREQ)
           + ((w.rem * CLOCK_FREQ) / sys->rate)
           - ((r.rem * CLOCK_FREQ) / freq)
           - ((GetQPC() - qpcpos) / (10000000 / CLOCK_FREQ));
    return hr;
}
