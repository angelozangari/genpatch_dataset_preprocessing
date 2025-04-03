}
static int64_t ParseTimeCode(uint8_t *data, double fps)
{
    return INT64_C(1000000) * (data[0] * 3600 +
                               data[1] *   60 +
                               data[2] *    1 +
                               data[3] /  fps);
}
