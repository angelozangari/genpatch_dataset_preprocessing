}
static int Open(vlc_object_t *obj)
{
    audio_output_t *aout = (audio_output_t *)obj;
    aout_sys_t *sys = malloc(sizeof (*sys));
    if (unlikely(sys == NULL))
        return VLC_ENOMEM;
    aout->sys = sys;
    aout->start = Start;
    aout->stop = Stop;
    aout->volume_set = WaveoutVolumeSet;
    aout->mute_set = WaveoutMuteSet;
    aout->device_select = DeviceSelect;
    sys->f_volume = var_InheritFloat(aout, "waveout-volume");
    sys->b_mute = var_InheritBool(aout, "mute");
    aout_MuteReport(aout, sys->b_mute);
    aout_VolumeReport(aout, sys->f_volume );
    if( vlc_timer_create( &sys->volume_poll_timer,
                          WaveoutPollVolume, aout ) )
    {
        msg_Err( aout, "Couldn't create volume polling timer" );
        free( sys );
        return VLC_ENOMEM;
    }
    vlc_mutex_init( &sys->lock );
    vlc_cond_init( &sys->cond );
    /* WaveOut does not support hot-plug events so list devices at startup */
    char **ids, **names;
    int count = ReloadWaveoutDevices(VLC_OBJECT(aout), NULL, &ids, &names);
    if (count >= 0)
    {
        for (int i = 0; i < count; i++)
        {
            aout_HotplugReport(aout, ids[i], names[i]);
            free(names[i]);
            free(ids[i]);
        }
        free(names);
        free(ids);
    }
    char *dev = var_CreateGetNonEmptyString(aout, "waveout-audio-device");
    aout_DeviceReport(aout, dev);
    free(dev);
    return VLC_SUCCESS;
}
