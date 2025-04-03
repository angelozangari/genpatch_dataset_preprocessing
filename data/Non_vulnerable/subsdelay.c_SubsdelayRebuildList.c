}
static void SubsdelayRebuildList( subsdelay_heap_t *p_heap )
{
    subsdelay_heap_entry_t *p_curr;
    int i_index;
    i_index = 0;
    for( p_curr = p_heap->p_head; p_curr != NULL; p_curr = p_curr->p_next )
    {
        p_heap->p_list[i_index] = p_curr;
        i_index++;
    }
    p_heap->i_count = i_index;
}
