static CRITICAL_SECTION clock_lock;
static mtime_t mdate_giveup (void)
{
    abort ();
}
