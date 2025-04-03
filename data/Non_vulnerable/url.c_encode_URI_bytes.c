}
static char *encode_URI_bytes (const char *str, size_t *restrict lenp)
{
    char *buf = malloc (3 * *lenp + 1);
    if (unlikely(buf == NULL))
        return NULL;
    char *out = buf;
    for (size_t i = 0; i < *lenp; i++)
    {
        static const char hex[16] = "0123456789ABCDEF";
        unsigned char c = str[i];
        if (isurisafe (c))
            *(out++) = c;
        /* This is URI encoding, not HTTP forms:
         * Space is encoded as '%20', not '+'. */
        else
        {
            *(out++) = '%';
            *(out++) = hex[c >> 4];
            *(out++) = hex[c & 0xf];
        }
    }
    *lenp = out - buf;
    out = realloc (buf, *lenp + 1);
    return likely(out != NULL) ? out : buf;
}
