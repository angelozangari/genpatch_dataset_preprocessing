 */
void vlc_UrlParse (vlc_url_t *restrict url, const char *str, unsigned char opt)
{
    url->psz_protocol = NULL;
    url->psz_username = NULL;
    url->psz_password = NULL;
    url->psz_host = NULL;
    url->i_port = 0;
    url->psz_path = NULL;
    url->psz_option = NULL;
    url->psz_buffer = NULL;
    if (str == NULL)
        return;
    char *buf = strdup (str);
    if (unlikely(buf == NULL))
        abort ();
    url->psz_buffer = buf;
    char *cur = buf, *next;
    /* URL scheme */
    next = buf;
    while ((*next >= 'A' && *next <= 'Z') || (*next >= 'a' && *next <= 'z')
        || (*next >= '0' && *next <= '9') || memchr ("+-.", *next, 3) != NULL)
        next++;
    /* This is not strictly correct. In principles, the scheme is always
     * present in an absolute URL and followed by a colon. Depending on the
     * URL scheme, the two subsequent slashes are not required.
     * VLC uses a different scheme for historical compatibility reasons - the
     * scheme is often implicit. */
    if (!strncmp (next, "://", 3))
    {
        *next = '\0';
        next += 3;
        url->psz_protocol = cur;
        cur = next;
    }
    /* Path */
    next = strchr (cur, '/');
    if (next != NULL)
    {
        *next = '\0'; /* temporary nul, reset to slash later */
        url->psz_path = next;
        if (opt && (next = strchr (next + 1, opt)) != NULL)
        {
            *(next++) = '\0';
            url->psz_option = next;
        }
    }
    /*else
        url->psz_path = "/";*/
    /* User name */
    next = strrchr (cur, '@');
    if (next != NULL)
    {
        *(next++) = '\0';
        url->psz_username = cur;
        cur = next;
        /* Password (obsolete) */
        next = strchr (url->psz_username, ':');
        if (next != NULL)
        {
            *(next++) = '\0';
            url->psz_password = next;
            decode_URI (url->psz_password);
        }
        decode_URI (url->psz_username);
    }
    /* Host name */
    if (*cur == '[' && (next = strrchr (cur, ']')) != NULL)
    {   /* Try IPv6 numeral within brackets */
        *(next++) = '\0';
        url->psz_host = strdup (cur + 1);
        if (*next == ':')
            next++;
        else
            next = NULL;
    }
    else
    {
        next = strchr (cur, ':');
        if (next != NULL)
            *(next++) = '\0';
        url->psz_host = vlc_idna_to_ascii (cur);
    }
    /* Port number */
    if (next != NULL)
        url->i_port = atoi (next);
    if (url->psz_path != NULL)
        *url->psz_path = '/'; /* restore leading slash */
}
