}
static HRESULT Start(aout_stream_t *s, audio_sample_format_t *restrict fmt,
                     const GUID *sid)
{
    if (!s->b_force && var_InheritBool(s, "spdif") && AOUT_FMT_SPDIF(fmt))
        /* Fallback to other plugin until pass-through is implemented */
        return E_NOTIMPL;
    static INIT_ONCE freq_once = INIT_ONCE_STATIC_INIT;
    if (!InitOnceExecuteOnce(&freq_once, InitFreq, &freq, NULL))
        return E_FAIL;
    aout_stream_sys_t *sys = malloc(sizeof (*sys));
    if (unlikely(sys == NULL))
        return E_OUTOFMEMORY;
    sys->client = NULL;
    void *pv;
    HRESULT hr = aout_stream_Activate(s, &IID_IAudioClient, NULL, &pv);
    if (FAILED(hr))
    {
        msg_Err(s, "cannot activate client (error 0x%lx)", hr);
        goto error;
    }
    sys->client = pv;
    /* Configure audio stream */
    WAVEFORMATEXTENSIBLE wf;
    WAVEFORMATEX *pwf;
    vlc_ToWave(&wf, fmt);
    hr = IAudioClient_IsFormatSupported(sys->client, AUDCLNT_SHAREMODE_SHARED,
                                        &wf.Format, &pwf);
    if (FAILED(hr))
    {
        msg_Err(s, "cannot negotiate audio format (error 0x%lx)", hr);
        goto error;
    }
    if (hr == S_FALSE)
    {
        assert(pwf != NULL);
        if (vlc_FromWave(pwf, fmt))
        {
            CoTaskMemFree(pwf);
            msg_Err(s, "unsupported audio format");
            hr = E_INVALIDARG;
            goto error;
        }
        msg_Dbg(s, "modified format");
    }
    else
        assert(pwf == NULL);
    sys->chans_to_reorder = vlc_CheckWaveOrder((hr == S_OK) ? &wf.Format : pwf,
                                               sys->chans_table);
    sys->format = fmt->i_format;
    hr = IAudioClient_Initialize(sys->client, AUDCLNT_SHAREMODE_SHARED, 0,
                                 AOUT_MAX_PREPARE_TIME * 10, 0,
                                 (hr == S_OK) ? &wf.Format : pwf, sid);
    CoTaskMemFree(pwf);
    if (FAILED(hr))
    {
        msg_Err(s, "cannot initialize audio client (error 0x%lx)", hr);
        goto error;
    }
    hr = IAudioClient_GetBufferSize(sys->client, &sys->frames);
    if (FAILED(hr))
    {
        msg_Err(s, "cannot get buffer size (error 0x%lx)", hr);
        goto error;
    }
    msg_Dbg(s, "buffer size    : %"PRIu32" frames", sys->frames);
    REFERENCE_TIME latT, defT, minT;
    if (SUCCEEDED(IAudioClient_GetStreamLatency(sys->client, &latT))
     && SUCCEEDED(IAudioClient_GetDevicePeriod(sys->client, &defT, &minT)))
    {
        msg_Dbg(s, "maximum latency: %"PRIu64"00 ns", latT);
        msg_Dbg(s, "default period : %"PRIu64"00 ns", defT);
        msg_Dbg(s, "minimum period : %"PRIu64"00 ns", minT);
    }
    sys->rate = fmt->i_rate;
    sys->bytes_per_frame = fmt->i_bytes_per_frame;
    sys->written = 0;
    s->sys = sys;
    s->time_get = TimeGet;
    s->play = Play;
    s->pause = Pause;
    s->flush = Flush;
    return S_OK;
error:
    if (sys->client != NULL)
        IAudioClient_Release(sys->client);
    free(sys);
    return hr;
}
