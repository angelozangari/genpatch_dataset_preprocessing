/* */
static int Lock(picture_t *picture)
{
    picture_sys_t *picsys = picture->p_sys;
    vout_display_sys_t *sys = picsys->sys;
    void *planes[PICTURE_PLANE_MAX];
    picsys->id = sys->lock(sys->opaque, planes);
    for (int i = 0; i < picture->i_planes; i++)
        picture->p[i].p_pixels = planes[i];
    return VLC_SUCCESS;
}
