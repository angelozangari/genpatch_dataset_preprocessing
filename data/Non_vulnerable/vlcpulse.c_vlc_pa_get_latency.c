 */
mtime_t vlc_pa_get_latency(vlc_object_t *obj, pa_context *ctx, pa_stream *s)
{
    /* NOTE: pa_stream_get_latency() will report 0 rather than negative latency
     * when the write index of a playback stream is behind its read index.
     * playback streams. So use the lower-level pa_stream_get_timing_info()
     * directly to obtain the correct write index and convert it to a time,
     * and compute the correct latency value by substracting the stream (read)
     * time.
     *
     * On the read side, pa_stream_get_time() is used instead of
     * pa_stream_get_timing_info() for the sake of interpolation. */
    const pa_sample_spec *ss = pa_stream_get_sample_spec(s);
    const pa_timing_info *ti = pa_stream_get_timing_info(s);
    if (ti == NULL) {
        msg_Dbg(obj, "no timing infos");
        return VLC_TS_INVALID;
    }
    if (ti->write_index_corrupt) {
        msg_Dbg(obj, "write index corrupt");
        return VLC_TS_INVALID;
    }
    pa_usec_t wt = pa_bytes_to_usec((uint64_t)ti->write_index, ss);
    pa_usec_t rt;
    if (pa_stream_get_time(s, &rt)) {
        if (pa_context_errno(ctx) != PA_ERR_NODATA)
            vlc_pa_error(obj, "unknown time", ctx);
        return VLC_TS_INVALID;
    }
    union { uint64_t u; int64_t s; } d;
    d.u = wt - rt;
    return d.s; /* non-overflowing unsigned to signed conversion */
}
