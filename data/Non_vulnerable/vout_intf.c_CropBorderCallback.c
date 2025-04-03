}
static int CropBorderCallback(vlc_object_t *object, char const *cmd,
                              vlc_value_t oldval, vlc_value_t newval, void *data)
{
    vout_thread_t *vout = (vout_thread_t *)object;
    VLC_UNUSED(cmd); VLC_UNUSED(oldval); VLC_UNUSED(data); VLC_UNUSED(newval);
    vout_ControlChangeCropBorder(vout,
                                 var_GetInteger(object, "crop-left"),
                                 var_GetInteger(object, "crop-top"),
                                 var_GetInteger(object, "crop-right"),
                                 var_GetInteger(object, "crop-bottom"));
    return VLC_SUCCESS;
}
