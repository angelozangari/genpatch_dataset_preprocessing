};
static int DemuxOnce (demux_t *demux, bool master)
{
    demux_sys_t *sys = demux->p_sys;
    mtime_t pts = date_Get (&sys->date);
    lldiv_t d;
    unsigned h, m, s, f;
    d = lldiv (pts, CLOCK_FREQ);
    f = d.rem * sys->date.i_divider_num / sys->date.i_divider_den / CLOCK_FREQ;
    d = lldiv (d.quot, 60);
    s = d.rem;
    d = lldiv (d.quot, 60);
    m = d.rem;
    h = d.quot;
    char *str;
    int len = asprintf (&str, "%02u:%02u:%02u:%02u", h, m, s, f);
    if (len == -1)
        return -1;
    block_t *block = block_heap_Alloc (str, len + 1);
    if (unlikely(block == NULL))
        return -1;
    block->i_buffer = len;
    assert(str[len] == '\0');
    block->i_pts = block->i_dts = pts;
    block->i_length = date_Increment (&sys->date, 1) - pts;
    es_out_Send (demux->out, sys->es, block);
    if (master)
        es_out_Control (demux->out, ES_OUT_SET_PCR, pts);
    return 1;
}
