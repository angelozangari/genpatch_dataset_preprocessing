}
static mtime_t mdate_wall (void)
{
    FILETIME ts;
    ULARGE_INTEGER s;
#if (_WIN32_WINNT >= 0x0602) && !VLC_WINSTORE_APP
    GetSystemTimePreciseAsFileTime (&ts);
#else
    GetSystemTimeAsFileTime (&ts);
#endif
    s.LowPart = ts.dwLowDateTime;
    s.HighPart = ts.dwHighDateTime;
    /* hundreds of nanoseconds */
    static_assert ((10000000 % CLOCK_FREQ) == 0, "Broken frequencies ratio");
    return s.QuadPart / (10000000 / CLOCK_FREQ);
}
