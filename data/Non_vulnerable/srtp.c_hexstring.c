}
static ssize_t hexstring (const char *in, uint8_t *out, size_t outlen)
{
    size_t inlen = strlen (in);
    if ((inlen > (2 * outlen)) || (inlen & 1))
        return -1;
    for (size_t i = 0; i < inlen; i += 2)
    {
        int a = hexdigit (in[i]), b = hexdigit (in[i + 1]);
        if ((a == -1) || (b == -1))
            return -1;
        out[i / 2] = (a << 4) | b;
    }
    return inlen / 2;
}
