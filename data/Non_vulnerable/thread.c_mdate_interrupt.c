} clk;
static mtime_t mdate_interrupt (void)
{
    ULONGLONG ts;
    BOOL ret;
#if (_WIN32_WINNT >= 0x0601)
    ret = QueryUnbiasedInterruptTime (&ts);
#else
    ret = clk.interrupt.query (&ts);
#endif
    if (unlikely(!ret))
        abort ();
    /* hundreds of nanoseconds */
    static_assert ((10000000 % CLOCK_FREQ) == 0, "Broken frequencies ratio");
    return ts / (10000000 / CLOCK_FREQ);
}
