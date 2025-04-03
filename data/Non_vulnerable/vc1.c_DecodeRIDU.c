/* DecodeRIDU: decode the startcode emulation prevention (same than h264) */
static void DecodeRIDU( uint8_t *p_ret, int *pi_ret, uint8_t *src, int i_src )
{
    uint8_t *end = &src[i_src];
    uint8_t *dst_end = &p_ret[*pi_ret];
    uint8_t *dst = p_ret;
    while( src < end && dst < dst_end )
    {
        if( src < end - 3 && src[0] == 0x00 && src[1] == 0x00 &&
            src[2] == 0x03 && dst < dst_end - 1 )
        {
            *dst++ = 0x00;
            *dst++ = 0x00;
            src += 3;
            continue;
        }
        *dst++ = *src++;
    }
    *pi_ret = dst - p_ret;
}
