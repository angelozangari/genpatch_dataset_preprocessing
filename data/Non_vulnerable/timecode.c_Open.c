}
static int Open (vlc_object_t *obj)
{
    demux_t *demux = (demux_t *)obj;
    demux_sys_t *sys = malloc (sizeof (*sys));
    if (unlikely(sys == NULL))
        return VLC_ENOMEM;
    es_format_t fmt;
    es_format_Init (&fmt, SPU_ES, VLC_CODEC_ITU_T140);
    sys->es = es_out_Add (demux->out, &fmt);
    unsigned num, den;
    if (var_InheritURational (demux, &num, &den, "timecode-fps")
     || !num || !den)
    {
        msg_Err (demux, "invalid frame rate");
        free (sys);
        return VLC_EGENERIC;
    }
    date_Init (&sys->date, num, den);
    date_Set (&sys->date, VLC_TS_0);
    sys->next_time = VLC_TS_INVALID;
    demux->p_sys = sys;
    demux->pf_demux   = Demux;
    demux->pf_control = Control;
    return VLC_SUCCESS;
}
