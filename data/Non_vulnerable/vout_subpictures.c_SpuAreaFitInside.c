}
static void SpuAreaFitInside(spu_area_t *area, const spu_area_t *boundary)
{
    spu_area_t a = spu_area_scaled(*area);
    const int i_error_x = (a.x + a.width) - boundary->width;
    if (i_error_x > 0)
        a.x -= i_error_x;
    if (a.x < 0)
        a.x = 0;
    const int i_error_y = (a.y + a.height) - boundary->height;
    if (i_error_y > 0)
        a.y -= i_error_y;
    if (a.y < 0)
        a.y = 0;
    *area = spu_area_unscaled(a, area->scale);
}
