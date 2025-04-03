#endif
int vlc_bindtextdomain (const char *domain)
{
#if defined (ENABLE_NLS)
    /* Specify where to find the locales for current domain */
# if !defined (__APPLE__) && !defined (_WIN32) && !defined(__OS2__)
    static const char path[] = LOCALEDIR;
    if (bindtextdomain (domain, path) == NULL)
    {
        fprintf (stderr, "%s: text domain not found in %s\n", domain, path);
        return -1;
    }
# else
    char *datadir = config_GetDataDir();
    if (unlikely(datadir == NULL))
        return -1;
    char *upath;
    int ret = asprintf (&upath, "%s" DIR_SEP "locale", datadir);
    free (datadir);
    if (unlikely(ret == -1))
        return -1;
    char *lpath = ToLocaleDup (upath);
    if (lpath == NULL || bindtextdomain (domain, lpath) == NULL)
    {
        free (lpath);
        fprintf (stderr, "%s: text domain not found in %s\n", domain, upath);
        free (upath);
        return -1;
    }
    free (lpath);
    free (upath);
# endif
    /* LibVLC wants all messages in UTF-8.
     * Unfortunately, we cannot ask UTF-8 for strerror_r(), strsignal_r()
     * and other functions that are not part of our text domain.
     */
    if (bind_textdomain_codeset (PACKAGE_NAME, "UTF-8") == NULL)
    {
        fprintf (stderr, "%s: UTF-8 encoding bot available\n", domain);
        // Unbinds the text domain to avoid broken encoding
        bindtextdomain (PACKAGE_NAME, "/DOES_NOT_EXIST");
        return -1;
    }
    /* LibVLC does NOT set the default textdomain, since it is a library.
     * This could otherwise break programs using LibVLC (other than VLC).
     * textdomain (PACKAGE_NAME);
     */
#else /* !ENABLE_NLS */
    (void)domain;
#endif
    return 0;
}
