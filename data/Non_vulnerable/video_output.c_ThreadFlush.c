}
static void ThreadFlush(vout_thread_t *vout, bool below, mtime_t date)
{
    vout->p->step.timestamp = VLC_TS_INVALID;
    vout->p->step.last      = VLC_TS_INVALID;
    ThreadFilterFlush(vout, false); /* FIXME too much */
    picture_t *last = vout->p->displayed.decoded;
    if (last) {
        if (( below && last->date <= date) ||
            (!below && last->date >= date)) {
            picture_Release(last);
            vout->p->displayed.decoded   = NULL;
            vout->p->displayed.date      = VLC_TS_INVALID;
            vout->p->displayed.timestamp = VLC_TS_INVALID;
        }
    }
    picture_fifo_Flush(vout->p->decoder_fifo, date, below);
}
