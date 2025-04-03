#endif
static mtime_t mdate_perf (void)
{
    /* We don't need the real date, just the value of a high precision timer */
    LARGE_INTEGER counter;
    if (!QueryPerformanceCounter (&counter))
        abort ();
    /* Convert to from (1/freq) to microsecond resolution */
    /* We need to split the division to avoid 63-bits overflow */
    lldiv_t d = lldiv (counter.QuadPart, clk.perf.freq.QuadPart);
    return (d.quot * 1000000) + ((d.rem * 1000000) / clk.perf.freq.QuadPart);
}
