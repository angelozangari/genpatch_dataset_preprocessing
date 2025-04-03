} smallest = { NULL, VLC_STATIC_MUTEX };
static int cmp_smallest (const void *a, const void *b)
{
    if (a == b)
        return 0;
    if (a == smallest.node)
        return -1;
    if (likely(b == smallest.node))
        return +1;
    abort ();
}
