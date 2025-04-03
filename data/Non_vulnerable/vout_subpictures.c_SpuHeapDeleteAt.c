}
static void SpuHeapDeleteAt(spu_heap_t *heap, int index)
{
    spu_heap_entry_t *e = &heap->entry[index];
    if (e->subpicture)
        subpicture_Delete(e->subpicture);
    e->subpicture = NULL;
}
