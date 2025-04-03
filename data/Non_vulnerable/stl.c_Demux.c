}
static int Demux(demux_t *demux)
{
    demux_sys_t *sys = demux->p_sys;
    while(sys->current < sys->count) {
        stl_entry_t *s = &sys->index[sys->current];
        if (s->start > sys->next_date)
            break;
        block_t *b = stream_Block(demux->s, 128 * s->count);
        if (b) {
            b->i_dts =
            b->i_pts = VLC_TS_0 + s->start;
            if (s->stop > s->start)
                b->i_length = s->stop - s->start;
            es_out_Send(demux->out, sys->es, b);
        }
        sys->current++;
    }
    return sys->current < sys->count ? 1 : 0;
}
