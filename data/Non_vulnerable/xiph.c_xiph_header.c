/* Convert configuration from RTP to VLC format */
static ssize_t xiph_header (void **pextra, const uint8_t *buf, size_t len)
{
    /* Headers number */
    if (len == 0)
          return -1; /* Invalid */
    unsigned hcount = 1 + *buf++;
    len--;
    if (hcount != 3)
          return -1; /* Invalid */
    /* Header lengths */
    uint16_t idlen = 0, cmtlen = 0, setuplen = 0;
    do
    {
        if (len == 0)
            return -1;
        idlen = (idlen << 7) | (*buf & 0x7f);
        len--;
    }
    while (*buf++ & 0x80);
    do
    {
        if (len == 0)
            return -1;
        cmtlen = (cmtlen << 7) | (*buf & 0x7f);
        len--;
    }
    while (*buf++ & 0x80);
    if (len < idlen + cmtlen)
        return -1;
    setuplen = len - (idlen + cmtlen);
    /* Create the VLC extra format header */
    unsigned sizes[3] = {
        idlen, cmtlen, setuplen
    };
    const void *payloads[3] = {
        buf + 0,
        buf + idlen,
        buf + idlen + cmtlen
    };
    void *extra;
    int  extra_size;
    if (xiph_PackHeaders (&extra_size, &extra, sizes, payloads, 3))
        return -1;;
    *pextra = extra;
    return extra_size;
}
