}
static int Control(demux_t *demux, int query, va_list args)
{
    demux_sys_t *sys = demux->p_sys;
    switch(query) {
    case DEMUX_GET_LENGTH: {
        int64_t *l = va_arg(args, int64_t *);
        *l = sys->count > 0 ? sys->index[sys->count-1].stop : 0;
        return VLC_SUCCESS;
    }
    case DEMUX_GET_TIME: {
        int64_t *t = va_arg(args, int64_t *);
        *t = sys->current < sys->count ? sys->index[sys->count-1].start : 0;
        return VLC_SUCCESS;
    }
    case DEMUX_SET_NEXT_DEMUX_TIME: {
        sys->next_date = va_arg(args, int64_t);
        return VLC_SUCCESS;
    }
    case DEMUX_SET_TIME: {
        int64_t t = va_arg(args, int64_t);
        sys->current = 0;
        while (sys->current < sys->count) {
            if (sys->index[sys->current].stop > t) {
                stream_Seek(demux->s, 1024 + 128LL * sys->index[sys->current].index);
                break;
            }
            sys->current++;
        }
        return VLC_SUCCESS;
    }
    case DEMUX_SET_POSITION:
    case DEMUX_GET_POSITION:
    default:
        return VLC_EGENERIC;
    }
}
