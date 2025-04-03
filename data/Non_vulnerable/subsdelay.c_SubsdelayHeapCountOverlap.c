 *****************************************************************************/
static int SubsdelayHeapCountOverlap( subsdelay_heap_t *p_heap, subsdelay_heap_entry_t *p_entry, mtime_t i_date )
{
    subsdelay_heap_entry_t *p_curr;
    int i_overlaps;
    VLC_UNUSED( p_heap );
    i_overlaps = 0;
    for( p_curr = p_entry->p_next; p_curr != NULL; p_curr = p_curr->p_next )
    {
        if( p_curr->p_source->i_start > i_date )
        {
            break;
        }
        if( !p_curr->b_check_empty ) /* subtitle was checked, and it's not empty */
        {
            i_overlaps++;
        }
    }
    return i_overlaps;
}
