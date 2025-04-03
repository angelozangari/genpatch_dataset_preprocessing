 */
static subpicture_region_t *OSDIcon(int type, const video_format_t *fmt)
{
    const float size_ratio   = 0.05;
    const float margin_ratio = 0.07;
    const int size   = __MAX(fmt->i_visible_width, fmt->i_visible_height);
    const int width  = size * size_ratio;
    const int height = size * size_ratio;
    const int x      = fmt->i_x_offset + fmt->i_visible_width - margin_ratio * size - width;
    const int y      = fmt->i_y_offset                        + margin_ratio * size;
    subpicture_region_t *r = OSDRegion(__MAX(x, 0),
                                       __MIN(y, (int)fmt->i_visible_height - height),
                                       width, height);
    if (!r)
        return NULL;
    if (type == OSD_PAUSE_ICON) {
        int bar_width = width / 3;
        DrawRect(r, STYLE_FILLED, 0, 0, bar_width - 1, height -1);
        DrawRect(r, STYLE_FILLED, width - bar_width, 0, width - 1, height - 1);
    } else if (type == OSD_PLAY_ICON) {
        int mid   = height >> 1;
        int delta = (width - mid) >> 1;
        int y2    = ((height - 1) >> 1) * 2;
        DrawTriangle(r, STYLE_FILLED, delta, 0, width - delta, y2);
    } else {
        int mid   = height >> 1;
        int delta = (width - mid) >> 1;
        int y2    = ((height - 1) >> 1) * 2;
        DrawRect(r, STYLE_FILLED, delta, mid / 2, width - delta, height - 1 - mid / 2);
        DrawTriangle(r, STYLE_FILLED, width - delta, 0, delta, y2);
        if (type == OSD_MUTE_ICON) {
            uint8_t *a    = r->p_picture->A_PIXELS;
            int     pitch = r->p_picture->A_PITCH;
            for (int i = 1; i < pitch; i++) {
                int k = i + (height - i - 1) * pitch;
                a[k] = 0xff - a[k];
            }
        }
    }
    return r;
}
