}
int64_t fixdiv64(int64_t x, int64_t y)
{
    int64_t temp;
    if(x == 0)
        return 0;
    if(y == 0)
        return 0x07ffffffffffffffLL;
    temp = x;
    temp <<= PRECISION64;
    return (int64_t)(temp / y);
}
