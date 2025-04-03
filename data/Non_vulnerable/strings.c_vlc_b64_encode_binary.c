/* Base64 encoding */
char *vlc_b64_encode_binary( const uint8_t *src, size_t i_src )
{
    static const char b64[] =
           "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    char *ret = malloc( ( i_src + 4 ) * 4 / 3 );
    char *dst = ret;
    if( dst == NULL )
        return NULL;
    while( i_src > 0 )
    {
        /* pops (up to) 3 bytes of input, push 4 bytes */
        uint32_t v;
        /* 1/3 -> 1/4 */
        v = *src++ << 24;
        *dst++ = b64[v >> 26];
        v = v << 6;
        /* 2/3 -> 2/4 */
        if( i_src >= 2 )
            v |= *src++ << 22;
        *dst++ = b64[v >> 26];
        v = v << 6;
        /* 3/3 -> 3/4 */
        if( i_src >= 3 )
            v |= *src++ << 20; // 3/3
        *dst++ = ( i_src >= 2 ) ? b64[v >> 26] : '='; // 3/4
        v = v << 6;
        /* -> 4/4 */
        *dst++ = ( i_src >= 3 ) ? b64[v >> 26] : '='; // 4/4
        if( i_src <= 3 )
            break;
        i_src -= 3;
    }
    *dst = '\0';
    return ret;
}
