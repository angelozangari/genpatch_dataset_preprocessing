};
static int TimeGet(audio_output_t *aout, mtime_t *restrict delay)
{
    aout_sys_t *sys = aout->sys;
    HRESULT hr;
    EnterMTA();
    hr = aout_stream_TimeGet(sys->stream, delay);
    LeaveMTA();
    return SUCCEEDED(hr) ? 0 : -1;
}
