 *****************************************************************************/
static subsdelay_heap_entry_t *SubsdelayHeapPush( subsdelay_heap_t *p_heap, subpicture_t *p_subpic, filter_t *p_filter )
{
    subsdelay_heap_entry_t *p_entry, *p_last, *p_new_entry;
    if( p_heap->i_count >= SUBSDELAY_MAX_ENTRIES )
    {
        return NULL; /* the heap is full */
    }
    p_new_entry = SubsdelayEntryCreate( p_subpic, p_filter );
    if( !p_new_entry )
    {
        return NULL;
    }
    p_last = NULL;
    for( p_entry = p_heap->p_head; p_entry != NULL; p_entry = p_entry->p_next )
    {
        if( p_entry->p_source->i_start > p_subpic->i_start )
        {
            /* the new entry should be inserted before p_entry */
            break;
        }
        p_last = p_entry;
    }
    if( p_last )
    {
        p_new_entry->p_next = p_last->p_next;
        p_last->p_next = p_new_entry;
        if( p_last->b_update_ephemer )
        {
            /* the correct stop value can be determined */
            p_last->p_source->i_stop = p_new_entry->p_source->i_start;
            p_last->b_update_ephemer = false;
        }
    }
    else
    {
        p_new_entry->p_next = p_heap->p_head;
        p_heap->p_head = p_new_entry;
    }
    /* rebuild list */
    SubsdelayRebuildList( p_heap );
    return p_new_entry;
}
