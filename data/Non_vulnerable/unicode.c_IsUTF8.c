 */
const char *IsUTF8( const char *str )
{
    size_t n;
    uint32_t cp;
    while ((n = vlc_towc (str, &cp)) != 0)
        if (likely(n != (size_t)-1))
            str += n;
        else
            return NULL;
    return str;
}
