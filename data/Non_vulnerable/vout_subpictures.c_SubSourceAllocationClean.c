}
static void SubSourceAllocationClean(filter_t *filter)
{
    filter_owner_sys_t *sys = filter->p_owner;
    spu_ClearChannel(sys->spu, sys->channel);
    free(filter->p_owner);
}
