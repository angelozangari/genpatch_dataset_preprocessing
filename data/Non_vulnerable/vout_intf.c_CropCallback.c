 *****************************************************************************/
static int CropCallback( vlc_object_t *object, char const *cmd,
                         vlc_value_t oldval, vlc_value_t newval, void *data )
{
    vout_thread_t *vout = (vout_thread_t *)object;
    VLC_UNUSED(cmd); VLC_UNUSED(oldval); VLC_UNUSED(data);
    unsigned num, den;
    unsigned y, x;
    unsigned width, height;
    unsigned left, top, right, bottom;
    if (sscanf(newval.psz_string, "%u:%u", &num, &den) == 2) {
        vout_ControlChangeCropRatio(vout, num, den);
    } else if (sscanf(newval.psz_string, "%ux%u+%u+%u",
                      &width, &height, &x, &y) == 4) {
        vout_ControlChangeCropWindow(vout, x, y, width, height);
    } else if (sscanf(newval.psz_string, "%u+%u+%u+%u",
                    &left, &top, &right, &bottom) == 4) {
        vout_ControlChangeCropBorder(vout, left, top, right, bottom);
    } else if (*newval.psz_string == '\0') {
        vout_ControlChangeCropRatio(vout, 0, 0);
    } else {
        msg_Err(object, "Unknown crop format (%s)", newval.psz_string);
    }
    return VLC_SUCCESS;
}
