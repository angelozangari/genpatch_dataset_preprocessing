#define CRC24_POLY 0x1864CFBL
static long crc_octets( uint8_t *octets, size_t len )
{
    long crc = CRC24_INIT;
    int i;
    while (len--)
    {
        crc ^= (*octets++) << 16;
        for (i = 0; i < 8; i++)
        {
            crc <<= 1;
            if (crc & 0x1000000)
                crc ^= CRC24_POLY;
        }
    }
    return crc & 0xFFFFFFL;
}
