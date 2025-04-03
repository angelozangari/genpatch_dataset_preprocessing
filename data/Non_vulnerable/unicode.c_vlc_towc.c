 */
size_t vlc_towc (const char *str, uint32_t *restrict pwc)
{
    uint8_t *ptr = (uint8_t *)str, c;
    uint32_t cp;
    assert (str != NULL);
    c = *ptr;
    if (unlikely(c > 0xF4))
        return -1;
    int charlen = clz8 (c ^ 0xFF);
    switch (charlen)
    {
        case 0: // 7-bit ASCII character -> short cut
            *pwc = c;
            return c != '\0';
        case 1: // continuation byte -> error
            return -1;
        case 2:
            if (unlikely(c < 0xC2)) // ASCII overlong
                return -1;
            cp = (c & 0x1F) << 6;
            break;
        case 3:
            cp = (c & 0x0F) << 12;
            break;
        case 4:
            cp = (c & 0x07) << 18;
            break;
        default:
            assert (0);
    }
    /* Unrolled continuation bytes decoding */
    switch (charlen)
    {
        case 4:
            c = *++ptr;
            if (unlikely((c & 0xC0) != 0x80)) // not a continuation byte
                return -1;
            cp |= (c & 0x3F) << 12;
            if (unlikely(cp >= 0x110000)) // beyond Unicode range
                return -1;
            /* fall through */
        case 3:
            c = *++ptr;
            if (unlikely((c & 0xC0) != 0x80)) // not a continuation byte
                return -1;
            cp |= (c & 0x3F) << 6;
            if (unlikely(cp >= 0xD800 && cp < 0xE000)) // UTF-16 surrogate
                return -1;
            if (unlikely(cp < (1u << (5 * charlen - 4)))) // non-ASCII overlong
                return -1;
            /* fall through */
        case 2:
            c = *++ptr;
            if (unlikely((c & 0xC0) != 0x80)) // not a continuation byte
                return -1;
            cp |= (c & 0x3F);
            break;
    }
    *pwc = cp;
    return charlen;
}
