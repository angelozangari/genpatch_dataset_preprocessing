}
static void Close(vlc_object_t *object)
{
    demux_t *demux = (demux_t*)object;
    demux_sys_t *sys = demux->p_sys;
    free(sys->index);
    free(sys);
}
