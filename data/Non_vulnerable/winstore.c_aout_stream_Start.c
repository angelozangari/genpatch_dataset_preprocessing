}
static int aout_stream_Start(void *func, va_list ap)
{
    aout_stream_start_t start = func;
    aout_stream_t *s = va_arg(ap, aout_stream_t *);
    audio_sample_format_t *fmt = va_arg(ap, audio_sample_format_t *);
    HRESULT *hr = va_arg(ap, HRESULT *);
    *hr = start(s, fmt, &GUID_VLC_AUD_OUT);
    return SUCCEEDED(*hr) ? VLC_SUCCESS : VLC_EGENERIC;
}
