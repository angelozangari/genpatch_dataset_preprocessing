#define EPG_PROGRAM_SIZE 0.03
static subpicture_region_t * vout_OSDEpgSlider(int x, int y,
                                               int width, int height,
                                               float ratio)
{
    /* Create a new subpicture region */
    video_palette_t palette = {
        .i_entries = 4,
        .palette = {
            [0] = { 0xff, 0x80, 0x80, 0x00 },
            [1] = { 0x00, 0x80, 0x80, 0x00 },
            [2] = { 0xff, 0x80, 0x80, 0xff },
            [3] = { 0x00, 0x80, 0x80, 0xff },
        },
    };
    video_format_t fmt;
    video_format_Init(&fmt, VLC_CODEC_YUVP);
    fmt.i_width  = fmt.i_visible_width  = width;
    fmt.i_height = fmt.i_visible_height = height;
    fmt.i_sar_num = 1;
    fmt.i_sar_den = 1;
    fmt.p_palette = &palette;
    subpicture_region_t *region = subpicture_region_New(&fmt);
    if (!region)
        return NULL;
    region->i_x = x;
    region->i_y = y;
    picture_t *picture = region->p_picture;
    ratio = VLC_CLIP(ratio, 0, 1);
    int filled_part_width = ratio * width;
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            /* Slider border. */
            bool is_outline = j == 0 || j == height - 1 ||
                              i == 0 || i == width  - 1;
            /* We can see the video through the part of the slider
               which corresponds to the leaving time. */
            bool is_border = j < 3 || j > height - 4 ||
                             i < 3 || i > width  - 4 ||
                             i < filled_part_width;
            picture->p->p_pixels[picture->p->i_pitch * j + i] = 2 * is_border + is_outline;
        }
    }
    return region;
}
