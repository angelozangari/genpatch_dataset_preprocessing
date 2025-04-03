 */
static subpicture_region_t *OSDRegion(int x, int y, int width, int height)
{
    video_palette_t palette = {
        .i_entries = 2,
        .palette = {
            [0] = { 0xff, 0x80, 0x80, 0x00 },
            [1] = { 0xff, 0x80, 0x80, 0xff },
        },
    };
    video_format_t fmt;
    video_format_Init(&fmt, VLC_CODEC_YUVP);
    fmt.i_width          =
    fmt.i_visible_width  = width;
    fmt.i_height         =
    fmt.i_visible_height = height;
    fmt.i_sar_num        = 1;
    fmt.i_sar_den        = 1;
    fmt.p_palette        = &palette;
    subpicture_region_t *r = subpicture_region_New(&fmt);
    if (!r)
        return NULL;
    r->i_x = x;
    r->i_y = y;
    memset(r->p_picture->p->p_pixels, 0, r->p_picture->p->i_pitch * height);
    return r;
}
