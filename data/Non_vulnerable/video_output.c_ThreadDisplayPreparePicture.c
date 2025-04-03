/* */
static int ThreadDisplayPreparePicture(vout_thread_t *vout, bool reuse, bool frame_by_frame)
{
    bool is_late_dropped = vout->p->is_late_dropped && !vout->p->pause.is_on && !frame_by_frame;
    vlc_mutex_lock(&vout->p->filter.lock);
    picture_t *picture = filter_chain_VideoFilter(vout->p->filter.chain_static, NULL);
    assert(!reuse || !picture);
    while (!picture) {
        picture_t *decoded;
        if (reuse && vout->p->displayed.decoded) {
            decoded = picture_Hold(vout->p->displayed.decoded);
        } else {
            decoded = picture_fifo_Pop(vout->p->decoder_fifo);
            if (decoded) {
                if (is_late_dropped && !decoded->b_force) {
                    const mtime_t predicted = mdate() + 0; /* TODO improve */
                    const mtime_t late = predicted - decoded->date;
                    if (late > VOUT_DISPLAY_LATE_THRESHOLD) {
                        msg_Warn(vout, "picture is too late to be displayed (missing %"PRId64" ms)", late/1000);
                        picture_Release(decoded);
                        vout_statistic_AddLost(&vout->p->statistic, 1);
                        continue;
                    } else if (late > 0) {
                        msg_Dbg(vout, "picture might be displayed late (missing %"PRId64" ms)", late/1000);
                    }
                }
                if (!VideoFormatIsCropArEqual(&decoded->format, &vout->p->filter.format))
                    ThreadChangeFilters(vout, &decoded->format, vout->p->filter.configuration, true);
            }
        }
        if (!decoded)
            break;
        reuse = false;
        if (vout->p->displayed.decoded)
            picture_Release(vout->p->displayed.decoded);
        vout->p->displayed.decoded       = picture_Hold(decoded);
        vout->p->displayed.timestamp     = decoded->date;
        vout->p->displayed.is_interlaced = !decoded->b_progressive;
        picture = filter_chain_VideoFilter(vout->p->filter.chain_static, decoded);
    }
    vlc_mutex_unlock(&vout->p->filter.lock);
    if (!picture)
        return VLC_EGENERIC;
    assert(!vout->p->displayed.next);
    if (!vout->p->displayed.current)
        vout->p->displayed.current = picture;
    else
        vout->p->displayed.next    = picture;
    return VLC_SUCCESS;
}
