}
static size_t read_mpi(uint8_t *dst, const uint8_t *buf, size_t buflen, size_t bits)
{
    if (buflen < 2)
        return 0;
    size_t n = mpi_len(buf);
    if (n * 8 > bits)
        return 0;
    n += 2;
    if (buflen < n)
        return 0;
    memcpy(dst, buf, n);
    return n;
}
