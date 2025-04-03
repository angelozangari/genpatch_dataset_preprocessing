}
static int spu_scale_h(int v, const spu_scale_t s)
{
    return v * s.h / SCALE_UNIT;
}
