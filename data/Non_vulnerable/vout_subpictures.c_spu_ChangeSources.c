}
void spu_ChangeSources(spu_t *spu, const char *filters)
{
    spu_private_t *sys = spu->p;
    vlc_mutex_lock(&sys->lock);
    free(sys->source_chain_update);
    sys->source_chain_update = strdup(filters);
    vlc_mutex_unlock(&sys->lock);
}
