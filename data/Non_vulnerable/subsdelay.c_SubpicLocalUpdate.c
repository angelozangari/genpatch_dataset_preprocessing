 *****************************************************************************/
static void SubpicLocalUpdate( subpicture_t* p_subpic, mtime_t i_ts )
{
    subsdelay_heap_entry_t *p_entry;
    subsdelay_heap_t *p_heap;
    filter_t *p_filter;
    int i_overlapping;
    p_entry = p_subpic->updater.p_sys;
    if( !p_entry || !p_entry->p_filter )
    {
        return;
    }
    p_filter = p_entry->p_filter;
    p_heap = &p_filter->p_sys->heap;
    SubsdelayHeapLock( p_heap );
    if( p_entry->b_check_empty && p_subpic->p_region )
    {
        if( SubsdelayIsTextEmpty( p_subpic->p_region->psz_html ) ||
            SubsdelayIsTextEmpty( p_subpic->p_region->psz_text ) )
        {
            /* remove empty subtitle */
            p_subpic->b_ephemer = false;
            p_subpic->i_stop = p_subpic->i_start;
            SubsdelayHeapRemove( p_heap, p_entry );
            SubsdelayHeapUnlock( p_heap );
            return;
        }
        p_entry->b_check_empty = false;
    }
    if( p_entry->b_update_stop && !p_entry->b_update_ephemer )
    {
        p_entry->i_new_stop = p_entry->p_source->i_start + SubsdelayEstimateDelay( p_filter, p_entry );
        p_entry->b_update_stop = false;
        SubsdelayEnforceDelayRules( p_filter );
    }
    i_overlapping = SubsdelayHeapCountOverlap( p_heap, p_entry, i_ts );
    p_subpic->i_alpha = SubsdelayCalculateAlpha( p_filter, i_overlapping, p_entry->p_source->i_alpha );
    if( p_entry->b_update_position )
    {
        p_subpic->b_absolute = false;
        if( p_subpic->p_region )
        {
            p_subpic->p_region->i_x = 0;
            p_subpic->p_region->i_y = 10;
            p_subpic->p_region->i_align = ( p_subpic->p_region->i_align & ( ~SUBPICTURE_ALIGN_MASK ) )
                    | SUBPICTURE_ALIGN_BOTTOM;
        }
        p_entry->b_update_position = false;
    }
    else if( p_entry->b_last_region_saved )
    {
        p_subpic->b_absolute = true;
        if( p_subpic->p_region )
        {
            p_subpic->p_region->i_x = p_entry->i_last_region_x;
            p_subpic->p_region->i_y = p_entry->i_last_region_y;
            p_subpic->p_region->i_align = p_entry->i_last_region_align;
        }
    }
    SubsdelayHeapUnlock( p_heap );
}
