 *****************************************************************************/
static void SpuHeapInit(spu_heap_t *heap)
{
    for (int i = 0; i < VOUT_MAX_SUBPICTURES; i++) {
        spu_heap_entry_t *e = &heap->entry[i];
        e->subpicture = NULL;
        e->reject     = false;
    }
}
