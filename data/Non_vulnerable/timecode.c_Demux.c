}
static int Demux (demux_t *demux)
{
    demux_sys_t *sys = demux->p_sys;
    if (sys->next_time == VLC_TS_INVALID) /* Master mode */
        return DemuxOnce (demux, true);
    /* Slave mode */
    while (sys->next_time > date_Get (&sys->date))
    {
        int val = DemuxOnce (demux, false);
        if (val <= 0)
            return val;
    }
    return 1;
}
