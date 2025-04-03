}
static int spu_scale_w(int v, const spu_scale_t s)
{
    return v * s.w / SCALE_UNIT;
}
