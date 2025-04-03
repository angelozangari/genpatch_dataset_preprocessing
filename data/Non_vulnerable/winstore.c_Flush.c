}
static void Flush(audio_output_t *aout, bool wait)
{
    aout_sys_t *sys = aout->sys;
    EnterMTA();
    aout_stream_Flush(sys->stream, wait);
    LeaveMTA();
}
