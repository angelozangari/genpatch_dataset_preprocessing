}
char *vlc_ngettext (const char *msgid, const char *plural, unsigned long n)
{
#ifdef ENABLE_NLS
    if (likely(*msgid))
        return dngettext (PACKAGE_NAME, msgid, plural, n);
#endif
    return (char *)((n == 1) ? msgid : plural);
}
