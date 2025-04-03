 */
char *vlc_gettext (const char *msgid)
{
#ifdef ENABLE_NLS
    if (likely(*msgid))
        return dgettext (PACKAGE_NAME, msgid);
#endif
    return (char *)msgid;
}
