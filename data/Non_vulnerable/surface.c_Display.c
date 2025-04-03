}
static void Display(vout_display_t *vd, picture_t *picture, subpicture_t *subpicture)
{
    VLC_UNUSED(vd);
    VLC_UNUSED(subpicture);
    /* refcount lowers to 0, and pool_cfg.unlock is called */
    picture_Release(picture);
}
