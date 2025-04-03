 */
aout_volume_t *aout_volume_New(vlc_object_t *parent,
                               const audio_replay_gain_t *gain)
{
    aout_volume_t *vol = vlc_custom_create(parent, sizeof (aout_volume_t),
                                           "volume");
    if (unlikely(vol == NULL))
        return NULL;
    vol->module = NULL;
    vol->output_factor = 1.f;
    //audio_volume_t *obj = &vol->object;
    /* Gain */
    if (gain != NULL)
        memcpy(&vol->replay_gain, gain, sizeof (vol->replay_gain));
    else
        memset(&vol->replay_gain, 0, sizeof (vol->replay_gain));
    var_AddCallback(parent, "audio-replay-gain-mode",
                    ReplayGainCallback, vol);
    var_TriggerCallback(parent, "audio-replay-gain-mode");
    return vol;
}
