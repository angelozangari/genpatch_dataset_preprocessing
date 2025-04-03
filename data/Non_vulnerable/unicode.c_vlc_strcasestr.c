 */
char *vlc_strcasestr (const char *haystack, const char *needle)
{
    ssize_t s;
    do
    {
        const char *h = haystack, *n = needle;
        for (;;)
        {
            uint32_t cph, cpn;
            s = vlc_towc (n, &cpn);
            if (s == 0)
                return (char *)haystack;
            if (unlikely(s < 0))
                return NULL;
            n += s;
            s = vlc_towc (h, &cph);
            if (s <= 0 || towlower (cph) != towlower (cpn))
                break;
            h += s;
        }
        s = vlc_towc (haystack, &(uint32_t) { 0 });
        haystack += s;
    }
    while (s > 0);
    return NULL;
}
