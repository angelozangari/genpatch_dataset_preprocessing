}
int IntFrom64(int64_t x)
{
    int res = 0;
    unsigned char *p = (unsigned char *)&x;
#ifdef ROCKBOX_BIG_ENDIAN
    res = p[5] | (p[4]<<8) | (p[3]<<16) | (p[2]<<24);
#else
    res = p[2] | (p[3]<<8) | (p[4]<<16) | (p[5]<<24);
#endif
    return res;
}
