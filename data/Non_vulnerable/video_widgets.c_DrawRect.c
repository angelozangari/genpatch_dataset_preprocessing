 */
static void DrawRect(subpicture_region_t *r, int fill,
                     int x1, int y1, int x2, int y2)
{
    uint8_t *p    = r->p_picture->p->p_pixels;
    int     pitch = r->p_picture->p->i_pitch;
    if (fill == STYLE_FILLED) {
        for (int y = y1; y <= y2; y++) {
            for (int x = x1; x <= x2; x++)
                p[x + pitch * y] = 1;
        }
    } else {
        for (int y = y1; y <= y2; y++) {
            p[x1 + pitch * y] = 1;
            p[x2 + pitch * y] = 1;
        }
        for (int x = x1; x <= x2; x++) {
            p[x + pitch * y1] = 1;
            p[x + pitch * y2] = 1;
        }
    }
}
