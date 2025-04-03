 */
static char *vlc_idna_to_ascii (const char *idn)
{
#if defined (HAVE_IDN)
    char *adn;
    if (idna_to_ascii_8z (idn, &adn, IDNA_ALLOW_UNASSIGNED) != IDNA_SUCCESS)
        return NULL;
    return adn;
#elif defined (_WIN32) && (_WIN32_WINNT >= 0x0601)
    char *ret = NULL;
    wchar_t *wide = ToWide (idn);
    if (wide == NULL)
        return NULL;
    int len = IdnToAscii (IDN_ALLOW_UNASSIGNED, wide, -1, NULL, 0);
    if (len == 0)
        goto error;
    wchar_t *buf = malloc (sizeof (*buf) * len);
    if (unlikely(buf == NULL))
        goto error;
    if (!IdnToAscii (IDN_ALLOW_UNASSIGNED, wide, -1, buf, len))
    {
        free (buf);
        goto error;
    }
    ret = FromWide (buf);
    free (buf);
error:
    free (wide);
    return ret;
#else
    /* No IDN support, filter out non-ASCII domain names */
    for (const char *p = idn; *p; p++)
        if (((unsigned char)*p) >= 0x80)
            return NULL;
    return strdup (idn);
#endif
}
