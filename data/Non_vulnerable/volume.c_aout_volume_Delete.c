 */
void aout_volume_Delete(aout_volume_t *vol)
{
    if (vol == NULL)
        return;
    audio_volume_t *obj = &vol->object;
    if (vol->module != NULL)
        module_unneed(obj, vol->module);
    var_DelCallback(obj->p_parent, "audio-replay-gain-mode",
                    ReplayGainCallback, vol);
    vlc_object_release(obj);
}
