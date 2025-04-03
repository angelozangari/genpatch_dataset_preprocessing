}
size_t EnumClockSource (vlc_object_t *obj, const char *var,
                        char ***vp, char ***np)
{
    const size_t max = 6;
    char **values = xmalloc (sizeof (*values) * max);
    char **names = xmalloc (sizeof (*names) * max);
    size_t n = 0;
#if (_WIN32_WINNT < 0x0601)
    DWORD version = LOWORD(GetVersion());
    version = (LOBYTE(version) << 8) | (HIBYTE(version) << 0);
#endif
    values[n] = xstrdup ("");
    names[n] = xstrdup (_("Auto"));
    n++;
#if (_WIN32_WINNT < 0x0601)
    if (version >= 0x0601)
#endif
    {
        values[n] = xstrdup ("interrupt");
        names[n] = xstrdup ("Interrupt time");
        n++;
    }
#if (_WIN32_WINNT < 0x0600)
    if (version >= 0x0600)
#endif
    {
        values[n] = xstrdup ("tick");
        names[n] = xstrdup ("Windows time");
        n++;
    }
#if !VLC_WINSTORE_APP
    values[n] = xstrdup ("multimedia");
    names[n] = xstrdup ("Multimedia timers");
    n++;
#endif
    values[n] = xstrdup ("perf");
    names[n] = xstrdup ("Performance counters");
    n++;
    values[n] = xstrdup ("wall");
    names[n] = xstrdup ("System time (DANGEROUS!)");
    n++;
    *vp = values;
    *np = names;
    (void) obj; (void) var;
    return n;
}
