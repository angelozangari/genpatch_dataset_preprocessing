}
static void write_duration(FILE *stream, int64_t duration)
{
    lldiv_t d;
    long long sec;
    duration /= CLOCK_FREQ;
    d = lldiv(duration, 60);
    sec = d.rem;
    d = lldiv(d.quot, 60);
    fprintf(stream, "%02lld:%02lld:%02lld", d.quot, d.rem, sec);
}
