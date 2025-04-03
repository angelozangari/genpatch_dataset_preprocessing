 *****************************************************************************/
static void SubsdelayHeapDestroy( subsdelay_heap_t *p_heap )
{
    subsdelay_heap_entry_t *p_entry;
    SubsdelayHeapLock( p_heap );
    for( p_entry = p_heap->p_head; p_entry != NULL; p_entry = p_entry->p_next )
    {
        p_entry->p_subpic->i_stop = p_entry->p_source->i_stop;
        p_entry->p_filter = NULL;
    }
    SubsdelayHeapUnlock( p_heap );
    vlc_mutex_destroy( &p_heap->lock );
}
