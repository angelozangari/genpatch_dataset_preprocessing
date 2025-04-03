}
static mtime_t mdate_tick (void)
{
#if (_WIN32_WINNT >= 0x0600)
    ULONGLONG ts = GetTickCount64 ();
#else
    ULONGLONG ts = clk.tick.get ();
#endif
    /* milliseconds */
    static_assert ((CLOCK_FREQ % 1000) == 0, "Broken frequencies ratio");
    return ts * (CLOCK_FREQ / 1000);
}
