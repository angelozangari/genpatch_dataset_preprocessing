 **************************/
static uint8_t bytereverse( int n )
{
    n = (((n >> 1) & 0x55) | ((n << 1) & 0xaa));
    n = (((n >> 2) & 0x33) | ((n << 2) & 0xcc));
    n = (((n >> 4) & 0x0f) | ((n << 4) & 0xf0));
    return n;
}
