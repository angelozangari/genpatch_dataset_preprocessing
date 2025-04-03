}
static bool spu_area_overlap(spu_area_t a, spu_area_t b)
{
    const int dx = 0;
    const int dy = 0;
    a = spu_area_scaled(a);
    b = spu_area_scaled(b);
    return __MAX(a.x - dx, b.x) < __MIN(a.x + a.width  + dx, b.x + b.width ) &&
           __MAX(a.y - dy, b.y) < __MIN(a.y + a.height + dy, b.y + b.height);
}
