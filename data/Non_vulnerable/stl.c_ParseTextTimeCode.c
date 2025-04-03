}
static int64_t ParseTextTimeCode(uint8_t *data, double fps)
{
    uint8_t tmp[4];
    for (int i = 0; i < 4; i++)
        tmp[i] = ParseInteger(&data[2 * i], 2);
    return ParseTimeCode(tmp, fps);
}
