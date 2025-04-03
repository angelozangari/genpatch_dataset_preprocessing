}
void spu_ClearChannel(spu_t *spu, int channel)
{
    spu_private_t *sys = spu->p;
    vlc_mutex_lock(&sys->lock);
    for (int i = 0; i < VOUT_MAX_SUBPICTURES; i++) {
        spu_heap_entry_t *entry = &sys->heap.entry[i];
        subpicture_t *subpic = entry->subpicture;
        if (!subpic)
            continue;
        if (subpic->i_channel != channel && (channel != -1 || subpic->i_channel == SPU_DEFAULT_CHANNEL))
            continue;
        /* You cannot delete subpicture outside of spu_SortSubpictures */
        entry->reject = true;
    }
    vlc_mutex_unlock(&sys->lock);
}
