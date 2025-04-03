}
static void Pause(audio_output_t *aout, bool paused, mtime_t date)
{
    aout_sys_t *sys = aout->sys;
    EnterMTA();
    aout_stream_Pause(sys->stream, paused);
    LeaveMTA();
    (void) date;
}
