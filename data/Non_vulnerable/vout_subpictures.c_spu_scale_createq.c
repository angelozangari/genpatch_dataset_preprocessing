}
static spu_scale_t spu_scale_createq(int64_t wn, int64_t wd, int64_t hn, int64_t hd)
{
    return spu_scale_create(wn * SCALE_UNIT / wd,
                            hn * SCALE_UNIT / hd);
}
