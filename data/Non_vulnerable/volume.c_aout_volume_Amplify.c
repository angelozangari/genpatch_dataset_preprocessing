 */
int aout_volume_Amplify(aout_volume_t *vol, block_t *block)
{
    if (unlikely(vol == NULL) || vol->module == NULL)
        return -1;
    float amp = vol->output_factor
              * vlc_atomic_load_float (&vol->gain_factor);
    vol->object.amplify(&vol->object, block, amp);
    return 0;
}
