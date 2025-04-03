}
static void Close(vlc_object_t *obj)
{
    audio_output_t *aout = (audio_output_t *)obj;
    aout_sys_t *sys = aout->sys;
    var_Destroy(aout, "waveout-audio-device");
    vlc_timer_destroy( sys->volume_poll_timer );
    vlc_cond_destroy( &sys->cond );
    vlc_mutex_destroy( &sys->lock );
    free(sys);
}
