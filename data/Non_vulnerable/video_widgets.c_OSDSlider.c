 */
static subpicture_region_t *OSDSlider(int type, int position,
                                      const video_format_t *fmt)
{
    const int size = __MAX(fmt->i_visible_width, fmt->i_visible_height);
    const int margin = size * 0.10;
    int x, y;
    int width, height;
    if (type == OSD_HOR_SLIDER) {
        width  = __MAX(fmt->i_visible_width - 2 * margin, 1);
        height = __MAX(fmt->i_visible_height * 0.05,      1);
        x      = __MIN(fmt->i_x_offset + margin, fmt->i_visible_width - width);
        y      = __MAX(fmt->i_y_offset + fmt->i_visible_height - margin, 0);
    } else {
        width  = __MAX(fmt->i_visible_width * 0.025,       1);
        height = __MAX(fmt->i_visible_height - 2 * margin, 1);
        x      = __MAX(fmt->i_x_offset + fmt->i_visible_width - margin, 0);
        y      = __MIN(fmt->i_y_offset + margin, fmt->i_visible_height - height);
    }
    subpicture_region_t *r = OSDRegion(x, y, width, height);
    if( !r)
        return NULL;
    if (type == OSD_HOR_SLIDER) {
        int pos_x = (width - 2) * position / 100;
        DrawRect(r, STYLE_FILLED, pos_x - 1, 2, pos_x + 1, height - 3);
        DrawRect(r, STYLE_EMPTY,  0,         0, width - 1, height - 1);
    } else {
        int pos_mid = height / 2;
        int pos_y   = height - (height - 2) * position / 100;
        DrawRect(r, STYLE_FILLED, 2,         pos_y,   width - 3, height - 3);
        DrawRect(r, STYLE_FILLED, 1,         pos_mid, 1,         pos_mid   );
        DrawRect(r, STYLE_FILLED, width - 2, pos_mid, width - 2, pos_mid   );
        DrawRect(r, STYLE_EMPTY,  0,         0,       width - 1, height - 1);
    }
    return r;
}
