}
int64_t vlm_Date(void)
{
    struct timeval tv;
    (void)gettimeofday( &tv, NULL );
    return tv.tv_sec * INT64_C(1000000) + tv.tv_usec;
}
