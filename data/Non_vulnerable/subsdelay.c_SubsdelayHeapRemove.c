 *****************************************************************************/
static void SubsdelayHeapRemove( subsdelay_heap_t *p_heap, subsdelay_heap_entry_t *p_entry )
{
    subsdelay_heap_entry_t *p_curr, *p_prev;
    p_prev = NULL;
    for( p_curr = p_heap->p_head; p_curr != NULL; p_curr = p_curr->p_next )
    {
        if( p_curr == p_entry )
        {
            break;
        }
        p_prev = p_curr;
    }
    if( p_prev )
    {
        p_prev->p_next = p_entry->p_next;
    }
    else
    {
        p_heap->p_head = p_entry->p_next;
    }
    p_entry->p_filter = NULL;
    SubsdelayRebuildList( p_heap );
}
