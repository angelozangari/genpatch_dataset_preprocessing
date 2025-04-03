}
int32_t fixdiv32(int32_t x, int32_t y)
{
    int64_t temp;
    if(x == 0)
        return 0;
    if(y == 0)
        return 0x7fffffff;
    temp = x;
    temp <<= PRECISION;
    return (int32_t)(temp / y);
}
