static LARGE_INTEGER freq; /* performance counters frequency */
static UINT64 GetQPC(void)
{
    LARGE_INTEGER counter;
    if (!QueryPerformanceCounter(&counter))
        abort();
    lldiv_t d = lldiv(counter.QuadPart, freq.QuadPart);
    return (d.quot * 10000000) + ((d.rem * 10000000) / freq.QuadPart);
}
