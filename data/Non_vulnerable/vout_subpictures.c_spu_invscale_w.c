}
static int spu_invscale_w(int v, const spu_scale_t s)
{
    return v * SCALE_UNIT / s.w;
}
