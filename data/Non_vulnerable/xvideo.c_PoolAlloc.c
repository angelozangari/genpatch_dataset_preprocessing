}
static void PoolAlloc (vout_display_t *vd, unsigned requested_count)
{
    vout_display_sys_t *p_sys = vd->sys;
    const uint32_t *pitches= xcb_xv_query_image_attributes_pitches (p_sys->att);
    const uint32_t *offsets= xcb_xv_query_image_attributes_offsets (p_sys->att);
    const unsigned num_planes= __MIN(p_sys->att->num_planes, PICTURE_PLANE_MAX);
    p_sys->data_size = p_sys->att->data_size;
    picture_resource_t res = { NULL };
    for (unsigned i = 0; i < num_planes; i++)
    {
        uint32_t data_size;
        data_size = (i < num_planes - 1) ? offsets[i+1] : p_sys->data_size;
        res.p[i].i_lines = (data_size - offsets[i]) / pitches[i];
        res.p[i].i_pitch = pitches[i];
    }
    picture_t *pic_array[MAX_PICTURES];
    requested_count = __MIN(requested_count, MAX_PICTURES);
    unsigned count;
    for (count = 0; count < requested_count; count++)
    {
        xcb_shm_seg_t seg = p_sys->shm ? xcb_generate_id (p_sys->conn) : 0;
        if (XCB_picture_Alloc (vd, &res, p_sys->data_size, p_sys->conn, seg))
            break;
        /* Allocate further planes as specified by XVideo */
        /* We assume that offsets[0] is zero */
        for (unsigned i = 1; i < num_planes; i++)
            res.p[i].p_pixels = res.p[0].p_pixels + offsets[i];
        if (p_sys->swap_uv)
        {   /* YVU: swap U and V planes */
            uint8_t *buf = res.p[2].p_pixels;
            res.p[2].p_pixels = res.p[1].p_pixels;
            res.p[1].p_pixels = buf;
        }
        pic_array[count] = XCB_picture_NewFromResource (&vd->fmt, &res);
        if (unlikely(pic_array[count] == NULL))
            break;
    }
    xcb_flush (p_sys->conn);
    if (count == 0)
        return;
    p_sys->pool = picture_pool_New (count, pic_array);
    if (unlikely(p_sys->pool == NULL))
        while (count > 0)
            picture_Release(pic_array[--count]);
}
