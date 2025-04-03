}
static int Open(vlc_object_t *obj)
{
    audio_output_t *aout = (audio_output_t *)obj;
    aout_sys_t *sys = malloc(sizeof (*sys));
    if (unlikely(sys == NULL))
        return VLC_ENOMEM;
    aout->sys = sys;
    sys->stream = NULL;
    sys->client = var_InheritAddress(aout, "mmdevice-audioclient");
    assert(sys->client != NULL);
    aout->start = Start;
    aout->stop = Stop;
    aout->time_get = TimeGet;
    aout->play = Play;
    aout->pause = Pause;
    aout->flush = Flush;
    return VLC_SUCCESS;
}
