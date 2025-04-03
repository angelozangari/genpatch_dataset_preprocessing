}
static spu_area_t spu_area_scaled(spu_area_t a)
{
    if (a.scale.w == SCALE_UNIT && a.scale.h == SCALE_UNIT)
        return a;
    a.x      = spu_scale_w(a.x,      a.scale);
    a.y      = spu_scale_h(a.y,      a.scale);
    a.width  = spu_scale_w(a.width,  a.scale);
    a.height = spu_scale_h(a.height, a.scale);
    a.scale = spu_scale_unit();
    return a;
}
