}
static void Stop(audio_output_t *aout)
{
    aout_sys_t *sys = aout->sys;
    assert (sys->stream != NULL);
    EnterMTA();
    vlc_module_unload(sys->module, aout_stream_Stop, sys->stream);
    LeaveMTA();
    vlc_object_release(sys->stream);
    sys->stream = NULL;
}
