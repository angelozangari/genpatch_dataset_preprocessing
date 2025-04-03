}
static int ReplayGainCallback (vlc_object_t *obj, char const *var,
                               vlc_value_t oldval, vlc_value_t val, void *data)
{
    aout_volume_t *vol = data;
    float multiplier = aout_ReplayGainSelect(obj, val.psz_string,
                                             &vol->replay_gain);
    vlc_atomic_store_float (&vol->gain_factor, multiplier);
    VLC_UNUSED(var); VLC_UNUSED(oldval);
    return VLC_SUCCESS;
}
