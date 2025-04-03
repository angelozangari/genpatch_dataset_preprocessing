}
static void SpuHeapClean(spu_heap_t *heap)
{
    for (int i = 0; i < VOUT_MAX_SUBPICTURES; i++) {
        spu_heap_entry_t *e = &heap->entry[i];
        if (e->subpicture)
            subpicture_Delete(e->subpicture);
    }
}
