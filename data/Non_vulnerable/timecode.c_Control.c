}
static int Control (demux_t *demux, int query, va_list args)
{
    demux_sys_t *sys = demux->p_sys;
    switch (query)
    {
        case DEMUX_GET_POSITION:
            *va_arg (args, float *) = 0.f;
            break;
        case DEMUX_GET_LENGTH:
            *va_arg (args, int64_t *) = INT64_C(0);
            break;
        case DEMUX_GET_TIME:
            *va_arg (args, int64_t *) = date_Get (&sys->date);
            break;
        case DEMUX_SET_TIME:
            date_Set (&sys->date, va_arg (args, int64_t));
            break;
        case DEMUX_SET_NEXT_DEMUX_TIME:
        {
            const mtime_t pts = va_arg (args, int64_t );
            if (sys->next_time == VLC_TS_INVALID) /* first invocation? */
            {
                date_Set (&sys->date, pts);
                date_Decrement (&sys->date, 1);
            }
            sys->next_time = pts;
            break;
        }
        case DEMUX_GET_PTS_DELAY:
        {
            int64_t *v = va_arg (args, int64_t *);
            *v = INT64_C(1000) * var_InheritInteger (demux, "live-caching");
            break;
        }
        case DEMUX_CAN_PAUSE:
        case DEMUX_CAN_CONTROL_PACE:
        case DEMUX_CAN_SEEK:
            *va_arg (args, bool *) = true;
            break;
        default:
            return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
