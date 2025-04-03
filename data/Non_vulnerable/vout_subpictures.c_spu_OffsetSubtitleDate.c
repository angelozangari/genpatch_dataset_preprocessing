}
void spu_OffsetSubtitleDate(spu_t *spu, mtime_t duration)
{
    spu_private_t *sys = spu->p;
    vlc_mutex_lock(&sys->lock);
    for (int i = 0; i < VOUT_MAX_SUBPICTURES; i++) {
        spu_heap_entry_t *entry = &sys->heap.entry[i];
        subpicture_t *current = entry->subpicture;
        if (current && current->b_subtitle) {
            if (current->i_start > 0)
                current->i_start += duration;
            if (current->i_stop > 0)
                current->i_stop  += duration;
        }
    }
    vlc_mutex_unlock(&sys->lock);
}
