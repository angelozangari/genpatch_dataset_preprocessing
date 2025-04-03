}
static int Open(vlc_object_t *object)
{
    demux_t *demux = (demux_t*)object;
    const uint8_t *peek;
    if (stream_Peek(demux->s, &peek, 11) != 11)
        return VLC_EGENERIC;
    bool is_stl_25 = !memcmp(&peek[3], "STL25.01", 8);
    bool is_stl_30 = !memcmp(&peek[3], "STL30.01", 8);
    if (!is_stl_25 && !is_stl_30)
        return VLC_EGENERIC;
    const double fps = is_stl_25 ? 25 : 30;
    uint8_t header[1024];
    if (stream_Read(demux->s, header, sizeof(header)) != sizeof(header)) {
        msg_Err(demux, "Incomplete EBU STL header");
        return VLC_EGENERIC;
    }
    const int cct = ParseInteger(&header[12], 2);
    const mtime_t program_start = ParseTextTimeCode(&header[256], fps);
    const int tti_count = ParseInteger(&header[238], 5);
    msg_Dbg(demux, "Detected EBU STL : CCT=%d TTI=%d start=%8.8s %"PRId64, cct, tti_count, &header[256], program_start);
    demux_sys_t *sys = xmalloc(sizeof(*sys));
    sys->next_date = 0;
    sys->current   = 0;
    sys->count     = 0;
    sys->index     = xcalloc(tti_count, sizeof(*sys->index));
    bool comment = false;
    stl_entry_t *s = &sys->index[0];
    s->count = 0;
    for (int i = 0; i < tti_count; i++) {
        uint8_t tti[16];
        if (stream_Read(demux->s, tti, 16) != 16 ||
            stream_Read(demux->s, NULL, 112) != 112) {
            msg_Warn(demux, "Incomplete EBU STL file");
            break;
        }
        const int ebn = tti[3];
        if (ebn >= 0xf0 && ebn <= 0xfd)
            continue;
        if (ebn == 0xfe)
            continue;
        if (s->count <= 0) {
            comment  = tti[15] != 0;
            s->start = ParseTimeCode(&tti[5], fps) - program_start;
            s->stop  = ParseTimeCode(&tti[9], fps) - program_start;
            s->index = i;
        }
        s->count++;
        if (ebn == 0xff && !comment)
            s = &sys->index[++sys->count];
        if (ebn == 0xff && sys->count < tti_count)
            s->count = 0;
    }
    if (sys->count > 0)
        stream_Seek(demux->s, 1024 + 128LL * sys->index[0].index);
    es_format_t fmt;
    es_format_Init(&fmt, SPU_ES, VLC_CODEC_EBU_STL);
    fmt.i_extra = sizeof(header);
    fmt.p_extra = header;
    sys->es = es_out_Add(demux->out, &fmt);
    fmt.i_extra = 0;
    fmt.p_extra = NULL;
    es_format_Clean(&fmt);
    demux->p_sys      = sys;
    demux->pf_demux   = Demux;
    demux->pf_control = Control;
    return VLC_SUCCESS;
}
