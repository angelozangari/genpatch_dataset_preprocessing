 */
void spu_PutSubpicture(spu_t *spu, subpicture_t *subpic)
{
    spu_private_t *sys = spu->p;
    /* Update sub-filter chain */
    vlc_mutex_lock(&sys->lock);
    char *chain_update = sys->filter_chain_update;
    sys->filter_chain_update = NULL;
    vlc_mutex_unlock(&sys->lock);
    bool is_left_empty = false;
    vlc_mutex_lock(&sys->filter_chain_lock);
    if (chain_update) {
        if (*chain_update) {
            filter_chain_Reset(sys->filter_chain, NULL, NULL);
            filter_chain_AppendFromString(spu->p->filter_chain, chain_update);
        }
        else if (filter_chain_GetLength(spu->p->filter_chain) > 0)
            filter_chain_Reset(sys->filter_chain, NULL, NULL);
        /* "sub-source"  was formerly "sub-filter", so now the "sub-filter"
        configuration may contain sub-filters or sub-sources configurations.
        if the filters chain was left empty it may indicate that it's a sub-source configuration */
        is_left_empty = (filter_chain_GetLength(spu->p->filter_chain) == 0);
    }
    vlc_mutex_unlock(&sys->filter_chain_lock);
    if (is_left_empty) {
        /* try to use the configuration as a sub-source configuration,
           but only if there is no 'source_chain_update' value and
           if only if 'chain_update' has a value */
        if (chain_update && *chain_update) {
            vlc_mutex_lock(&sys->lock);
            if (!sys->source_chain_update || !*sys->source_chain_update) {
                if (sys->source_chain_update)
                    free(sys->source_chain_update);
                sys->source_chain_update = chain_update;
                chain_update = NULL;
            }
            vlc_mutex_unlock(&sys->lock);
        }
    }
    free(chain_update);
    /* Run filter chain on the new subpicture */
    vlc_mutex_lock(&sys->filter_chain_lock);
    subpic = filter_chain_SubFilter(spu->p->filter_chain, subpic);
    vlc_mutex_unlock(&sys->filter_chain_lock);
    if (!subpic)
        return;
    /* SPU_DEFAULT_CHANNEL always reset itself */
    if (subpic->i_channel == SPU_DEFAULT_CHANNEL)
        spu_ClearChannel(spu, SPU_DEFAULT_CHANNEL);
    /* p_private is for spu only and cannot be non NULL here */
    for (subpicture_region_t *r = subpic->p_region; r != NULL; r = r->p_next)
        assert(r->p_private == NULL);
    /* */
    vlc_mutex_lock(&sys->lock);
    if (SpuHeapPush(&sys->heap, subpic)) {
        vlc_mutex_unlock(&sys->lock);
        msg_Err(spu, "subpicture heap full");
        subpicture_Delete(subpic);
        return;
    }
    vlc_mutex_unlock(&sys->lock);
}
