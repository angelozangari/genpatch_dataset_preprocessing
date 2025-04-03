 *****************************************************************************/
static void UpdateSPU(spu_t *spu, vlc_object_t *object)
{
    spu_private_t *sys = spu->p;
    vlc_value_t val;
    vlc_mutex_lock(&sys->lock);
    sys->force_palette = false;
    sys->force_crop = false;
    if (var_Get(object, "highlight", &val) || !val.b_bool) {
        vlc_mutex_unlock(&sys->lock);
        return;
    }
    sys->force_crop = true;
    sys->crop.x      = var_GetInteger(object, "x-start");
    sys->crop.y      = var_GetInteger(object, "y-start");
    sys->crop.width  = var_GetInteger(object, "x-end") - sys->crop.x;
    sys->crop.height = var_GetInteger(object, "y-end") - sys->crop.y;
    if (var_Get(object, "menu-palette", &val) == VLC_SUCCESS) {
        memcpy(sys->palette, val.p_address, 16);
        sys->force_palette = true;
    }
    vlc_mutex_unlock(&sys->lock);
    msg_Dbg(object, "crop: %i,%i,%i,%i, palette forced: %i",
            sys->crop.x, sys->crop.y,
            sys->crop.width, sys->crop.height,
            sys->force_palette);
}
