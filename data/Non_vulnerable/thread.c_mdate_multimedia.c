#if !VLC_WINSTORE_APP
static mtime_t mdate_multimedia (void)
{
    DWORD ts = clk.multimedia.timeGetTime ();
    /* milliseconds */
    static_assert ((CLOCK_FREQ % 1000) == 0, "Broken frequencies ratio");
    return ts * (CLOCK_FREQ / 1000);
}
