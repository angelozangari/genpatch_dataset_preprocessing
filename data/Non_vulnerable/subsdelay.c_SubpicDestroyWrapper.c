 *****************************************************************************/
static void SubpicDestroyWrapper( subpicture_t *p_subpic )
{
    subsdelay_heap_entry_t *p_entry;
    subsdelay_heap_t *p_heap;
    p_entry = p_subpic->updater.p_sys;
    if( !p_entry )
    {
        return;
    }
    if( p_entry->p_filter )
    {
        p_heap = &p_entry->p_filter->p_sys->heap;
        SubsdelayHeapLock( p_heap );
        SubsdelayHeapRemove( p_heap, p_entry );
        SubsdelayHeapUnlock( p_heap );
    }
    SubsdelayEntryDestroy( p_entry );
}
