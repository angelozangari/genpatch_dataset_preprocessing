}
void aout_volume_SetVolume(aout_volume_t *vol, float factor)
{
    if (unlikely(vol == NULL))
        return;
    vol->output_factor = factor;
}
