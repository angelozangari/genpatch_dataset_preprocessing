 */
char *vlc_path2uri (const char *path, const char *scheme)
{
    if (path == NULL)
    {
        errno = EINVAL;
        return NULL;
    }
    if (scheme == NULL && !strcmp (path, "-"))
        return strdup ("fd://0"); // standard input
    /* Note: VLC cannot handle URI schemes without double slash after the
     * scheme name (such as mailto: or news:). */
    char *buf;
#ifdef __OS2__
    char p[strlen (path) + 1];
    for (buf = p; *path; buf++, path++)
        *buf = (*path == '/') ? DIR_SEP_CHAR : *path;
    *buf = '\0';
    path = p;
#endif
#if defined( _WIN32 ) || defined( __OS2__ )
    /* Drive letter */
    if (isalpha ((unsigned char)path[0]) && (path[1] == ':'))
    {
        if (asprintf (&buf, "%s:///%c:", scheme ? scheme : "file",
                      path[0]) == -1)
            buf = NULL;
        path += 2;
# warning Drive letter-relative path not implemented!
        if (path[0] != DIR_SEP_CHAR)
        {
            errno = ENOTSUP;
            return NULL;
        }
    }
    else
#endif
    if (!strncmp (path, "\\\\", 2))
    {   /* Windows UNC paths */
#if !defined( _WIN32 ) && !defined( __OS2__ )
        if (scheme != NULL)
        {
            errno = ENOTSUP;
            return NULL; /* remote files not supported */
        }
        /* \\host\share\path -> smb://host/share/path */
        if (strchr (path + 2, '\\') != NULL)
        {   /* Convert backslashes to slashes */
            char *dup = strdup (path);
            if (dup == NULL)
                return NULL;
            for (size_t i = 2; dup[i]; i++)
                if (dup[i] == '\\')
                    dup[i] = DIR_SEP_CHAR;
            char *ret = vlc_path2uri (dup, scheme);
            free (dup);
            return ret;
        }
# define SMB_SCHEME "smb"
#else
        /* \\host\share\path -> file://host/share/path */
# define SMB_SCHEME "file"
#endif
        size_t hostlen = strcspn (path + 2, DIR_SEP);
        buf = malloc (sizeof (SMB_SCHEME) + 3 + hostlen);
        if (buf != NULL)
            snprintf (buf, sizeof (SMB_SCHEME) + 3 + hostlen,
                      SMB_SCHEME"://%s", path + 2);
        path += 2 + hostlen;
        if (path[0] == '\0')
            return buf; /* Hostname without path */
    }
    else
    if (path[0] != DIR_SEP_CHAR)
    {   /* Relative path: prepend the current working directory */
        char *cwd, *ret;
        if ((cwd = vlc_getcwd ()) == NULL)
            return NULL;
        if (asprintf (&buf, "%s"DIR_SEP"%s", cwd, path) == -1)
            buf = NULL;
        free (cwd);
        ret = (buf != NULL) ? vlc_path2uri (buf, scheme) : NULL;
        free (buf);
        return ret;
    }
    else
    if (asprintf (&buf, "%s://", scheme ? scheme : "file") == -1)
        buf = NULL;
    if (buf == NULL)
        return NULL;
    /* Absolute file path */
    assert (path[0] == DIR_SEP_CHAR);
    do
    {
        size_t len = strcspn (++path, DIR_SEP);
        path += len;
        char *component = encode_URI_bytes (path - len, &len);
        if (unlikely(component == NULL))
        {
            free (buf);
            return NULL;
        }
        component[len] = '\0';
        char *uri;
        int val = asprintf (&uri, "%s/%s", buf, component);
        free (component);
        free (buf);
        if (unlikely(val == -1))
            return NULL;
        buf = uri;
    }
    while (*path);
    return buf;
}
