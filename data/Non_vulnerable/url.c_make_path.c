 */
char *make_path (const char *url)
{
    char *ret = NULL;
    char *end;
    char *path = strstr (url, "://");
    if (path == NULL)
        return NULL; /* unsupported scheme or invalid syntax */
    end = memchr (url, '/', path - url);
    size_t schemelen = ((end != NULL) ? end : path) - url;
    path += 3; /* skip "://" */
    /* Remove HTML anchor if present */
    end = strchr (path, '#');
    if (end)
        path = strndup (path, end - path);
    else
        path = strdup (path);
    if (unlikely(path == NULL))
        return NULL; /* boom! */
    /* Decode path */
    decode_URI (path);
    if (schemelen == 4 && !strncasecmp (url, "file", 4))
    {
#if !defined (_WIN32) && !defined (__OS2__)
        /* Leading slash => local path */
        if (*path == '/')
            return path;
        /* Local path disguised as a remote one */
        if (!strncasecmp (path, "localhost/", 10))
            return memmove (path, path + 9, strlen (path + 9) + 1);
#else
        /* cannot start with a space */
        if (*path == ' ')
            goto out;
        for (char *p = strchr (path, '/'); p; p = strchr (p + 1, '/'))
            *p = '\\';
        /* Leading backslash => local path */
        if (*path == '\\')
            return memmove (path, path + 1, strlen (path + 1) + 1);
        /* Local path disguised as a remote one */
        if (!strncasecmp (path, "localhost\\", 10))
            return memmove (path, path + 10, strlen (path + 10) + 1);
        /* UNC path */
        if (*path && asprintf (&ret, "\\\\%s", path) == -1)
            ret = NULL;
#endif
        /* non-local path :-( */
    }
    else
    if (schemelen == 2 && !strncasecmp (url, "fd", 2))
    {
        int fd = strtol (path, &end, 0);
        if (*end)
            goto out;
#if !defined( _WIN32 ) && !defined( __OS2__ )
        switch (fd)
        {
            case 0:
                ret = strdup ("/dev/stdin");
                break;
            case 1:
                ret = strdup ("/dev/stdout");
                break;
            case 2:
                ret = strdup ("/dev/stderr");
                break;
            default:
                if (asprintf (&ret, "/dev/fd/%d", fd) == -1)
                    ret = NULL;
        }
#else
        /* XXX: Does this work on WinCE? */
        if (fd < 2)
            ret = strdup ("CON");
        else
            ret = NULL;
#endif
    }
out:
    free (path);
    return ret; /* unknown scheme */
}
