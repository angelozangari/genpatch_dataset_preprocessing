 */
int aout_volume_SetFormat(aout_volume_t *vol, vlc_fourcc_t format)
{
    if (unlikely(vol == NULL))
        return -1;
    audio_volume_t *obj = &vol->object;
    if (vol->module != NULL)
    {
        if (obj->format == format)
        {
            msg_Dbg (obj, "retaining sample format");
            return 0;
        }
        msg_Dbg (obj, "changing sample format");
        module_unneed(obj, vol->module);
    }
    obj->format = format;
    vol->module = module_need(obj, "audio volume", NULL, false);
    if (vol->module == NULL)
        return -1;
    return 0;
}
