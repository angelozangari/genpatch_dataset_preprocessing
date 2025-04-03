}
static void Display(vout_display_t *vd, picture_t *picture, subpicture_t *subpicture)
{
    vout_display_sys_t *sys = vd->sys;
    void *id = picture->p_sys->id;
    assert(!picture_IsReferenced(picture));
    picture_Release(picture);
    if (sys->display != NULL)
        sys->display(sys->opaque, id);
    VLC_UNUSED(subpicture);
}
