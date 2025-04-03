} spu_scale_t;
static spu_scale_t spu_scale_create(int w, int h)
{
    spu_scale_t s = { .w = w, .h = h };
    if (s.w <= 0)
        s.w = SCALE_UNIT;
    if (s.h <= 0)
        s.h = SCALE_UNIT;
    return s;
}
