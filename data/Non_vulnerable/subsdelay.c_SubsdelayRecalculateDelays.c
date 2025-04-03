 *****************************************************************************/
static void SubsdelayRecalculateDelays( filter_t *p_filter )
{
    subsdelay_heap_entry_t *p_curr;
    for( p_curr = p_filter->p_sys->heap.p_head; p_curr != NULL; p_curr = p_curr->p_next )
    {
        if( !p_curr->b_update_ephemer )
        {
            p_curr->i_new_stop = p_curr->p_source->i_start + SubsdelayEstimateDelay( p_filter, p_curr );
            p_curr->b_update_stop = false;
        }
    }
    SubsdelayEnforceDelayRules( p_filter );
}
