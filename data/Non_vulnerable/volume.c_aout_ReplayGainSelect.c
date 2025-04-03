/*** Replay gain ***/
static float aout_ReplayGainSelect(vlc_object_t *obj, const char *str,
                                   const audio_replay_gain_t *replay_gain)
{
    unsigned mode = AUDIO_REPLAY_GAIN_MAX;
    if (likely(str != NULL))
    {   /* Find selectrf mode */
        if (!strcmp (str, "track"))
            mode = AUDIO_REPLAY_GAIN_TRACK;
        else
        if (!strcmp (str, "album"))
            mode = AUDIO_REPLAY_GAIN_ALBUM;
    }
    /* */
    float multiplier;
    if (mode == AUDIO_REPLAY_GAIN_MAX)
    {
        multiplier = 1.f;
    }
    else
    {
        float gain;
        /* If the selectrf mode is not available, prefer the other one */
        if (!replay_gain->pb_gain[mode] && replay_gain->pb_gain[!mode])
            mode = !mode;
        if (replay_gain->pb_gain[mode])
            gain = replay_gain->pf_gain[mode]
                 + var_InheritFloat (obj, "audio-replay-gain-preamp");
        else
            gain = var_InheritFloat (obj, "audio-replay-gain-default");
        multiplier = powf (10.f, gain / 20.f);
        if (var_InheritBool (obj, "audio-replay-gain-peak-protection"))
            multiplier = fminf (multiplier, replay_gain->pb_peak[mode]
                                            ? 1.f / replay_gain->pf_peak[mode]
                                            : 1.f);
    }
    /* Command line / configuration gain */
    multiplier *= var_InheritFloat (obj, "gain");
    return multiplier;
}
