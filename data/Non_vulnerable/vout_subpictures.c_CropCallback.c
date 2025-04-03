 *****************************************************************************/
static int CropCallback(vlc_object_t *object, char const *var,
                        vlc_value_t oldval, vlc_value_t newval, void *data)
{
    VLC_UNUSED(oldval); VLC_UNUSED(newval); VLC_UNUSED(var);
    UpdateSPU((spu_t *)data, object);
    return VLC_SUCCESS;
}
