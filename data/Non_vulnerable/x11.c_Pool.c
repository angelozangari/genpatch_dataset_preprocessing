 */
static picture_pool_t *Pool (vout_display_t *vd, unsigned requested_count)
{
    vout_display_sys_t *sys = vd->sys;
    (void)requested_count;
    if (sys->pool)
        return sys->pool;
    vout_display_place_t place;
    vout_display_PlacePicture (&place, &vd->source, vd->cfg, false);
    /* */
    const uint32_t values[] = { place.x, place.y, place.width, place.height };
    xcb_configure_window (sys->conn, sys->window,
                          XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y |
                          XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT,
                          values);
    picture_t *pic = picture_NewFromFormat (&vd->fmt);
    if (!pic)
        return NULL;
    assert (pic->i_planes == 1);
    picture_resource_t res = {
       .p = {
           [0] = {
               .i_lines = pic->p->i_lines,
               .i_pitch = pic->p->i_pitch,
           },
       },
    };
    picture_Release (pic);
    unsigned count;
    picture_t *pic_array[MAX_PICTURES];
    const size_t size = res.p->i_pitch * res.p->i_lines;
    for (count = 0; count < MAX_PICTURES; count++)
    {
        xcb_shm_seg_t seg = (sys->seg_base != 0) ? (sys->seg_base + count) : 0;
        if (XCB_picture_Alloc (vd, &res, size, sys->conn, seg))
            break;
        pic_array[count] = XCB_picture_NewFromResource (&vd->fmt, &res);
        if (unlikely(pic_array[count] == NULL))
        {
            if (seg != 0)
                xcb_shm_detach (sys->conn, seg);
            break;
        }
    }
    xcb_flush (sys->conn);
    if (count == 0)
        return NULL;
    sys->pool = picture_pool_New (count, pic_array);
    if (unlikely(sys->pool == NULL))
        while (count > 0)
            picture_Release(pic_array[--count]);
    return sys->pool;
}
