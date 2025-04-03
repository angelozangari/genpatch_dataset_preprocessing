 */
char *convert_xml_special_chars (const char *str)
{
    assert (str != NULL);
    const size_t len = strlen (str);
    char *const buf = malloc (6 * len + 1), *ptr = buf;
    if (unlikely(buf == NULL))
        return NULL;
    size_t n;
    uint32_t cp;
    while ((n = vlc_towc (str, &cp)) != 0)
    {
        if (unlikely(n == (size_t)-1))
        {
            free (buf);
            errno = EILSEQ;
            return NULL;
        }
        switch (cp)
        {
            case '\"': strcpy (ptr, "&quot;"); ptr += 6; break;
            case '&':  strcpy (ptr, "&amp;");  ptr += 5; break;
            case '\'': strcpy (ptr, "&#39;");  ptr += 5; break;
            case '<':  strcpy (ptr, "&lt;");   ptr += 4; break;
            case '>':  strcpy (ptr, "&gt;");   ptr += 4; break;
            default:
                if (cp < 32) /* C0 code not allowed (except 9, 10 and 13) */
                    break;
                if (cp >= 128 && cp < 160) /* C1 code encoded (except 133) */
                {
                    ptr += sprintf (ptr, "&#%"PRIu32";", cp);
                    break;
                }
                /* fall through */
            case 9:
            case 10:
            case 13:
            case 133:
                memcpy (ptr, str, n);
                ptr += n;
                break;
        }
        str += n;
    }
    *(ptr++) = '\0';
    ptr = realloc (buf, ptr - buf);
    return likely(ptr != NULL) ? ptr : buf; /* cannot fail */
}
