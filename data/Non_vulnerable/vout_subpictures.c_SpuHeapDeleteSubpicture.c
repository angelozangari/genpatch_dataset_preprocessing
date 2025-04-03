}
static int SpuHeapDeleteSubpicture(spu_heap_t *heap, subpicture_t *subpic)
{
    for (int i = 0; i < VOUT_MAX_SUBPICTURES; i++) {
        spu_heap_entry_t *e = &heap->entry[i];
        if (e->subpicture != subpic)
            continue;
        SpuHeapDeleteAt(heap, i);
        return VLC_SUCCESS;
    }
    return VLC_EGENERIC;
}
