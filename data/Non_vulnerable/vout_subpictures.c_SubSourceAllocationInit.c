}
static int SubSourceAllocationInit(filter_t *filter, void *data)
{
    spu_t *spu = data;
    filter_owner_sys_t *sys = malloc(sizeof(*sys));
    if (!sys)
        return VLC_EGENERIC;
    filter->pf_sub_buffer_new = sub_new_buffer;
    filter->pf_sub_buffer_del = sub_del_buffer;
    filter->p_owner = sys;
    sys->channel = spu_RegisterChannel(spu);
    sys->spu     = spu;
    return VLC_SUCCESS;
}
