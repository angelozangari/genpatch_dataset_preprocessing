}
static void ThreadChangePause(vout_thread_t *vout, bool is_paused, mtime_t date)
{
    assert(!vout->p->pause.is_on || !is_paused);
    if (vout->p->pause.is_on) {
        const mtime_t duration = date - vout->p->pause.date;
        if (vout->p->step.timestamp > VLC_TS_INVALID)
            vout->p->step.timestamp += duration;
        if (vout->p->step.last > VLC_TS_INVALID)
            vout->p->step.last += duration;
        picture_fifo_OffsetDate(vout->p->decoder_fifo, duration);
        if (vout->p->displayed.decoded)
            vout->p->displayed.decoded->date += duration;
        spu_OffsetSubtitleDate(vout->p->spu, duration);
        ThreadFilterFlush(vout, false);
    } else {
        vout->p->step.timestamp = VLC_TS_INVALID;
        vout->p->step.last      = VLC_TS_INVALID;
    }
    vout->p->pause.is_on = is_paused;
    vout->p->pause.date  = date;
}
