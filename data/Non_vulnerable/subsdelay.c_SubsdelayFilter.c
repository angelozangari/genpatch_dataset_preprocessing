 *****************************************************************************/
static subpicture_t * SubsdelayFilter( filter_t *p_filter, subpicture_t* p_subpic )
{
    subsdelay_heap_t *p_heap;
    subsdelay_heap_entry_t *p_entry;
    if( !p_subpic->b_subtitle )
    {
        return p_subpic;
    }
    if( SubpicIsEmpty( p_subpic ) )
    {
        /* empty subtitles usually helps terminate ephemer subtitles, but this filter calculates the stop value anyway,
           so this subtitle can be dropped */
        subpicture_Delete( p_subpic );
        return NULL;
    }
    p_heap = &p_filter->p_sys->heap;
    /* add subpicture to the heap */
    SubsdelayHeapLock( p_heap );
    p_entry = SubsdelayHeapPush( p_heap, p_subpic, p_filter );
    if( !p_entry )
    {
        SubsdelayHeapUnlock( p_heap );
        msg_Err(p_filter, "Can't add subpicture to the heap");
        return p_subpic;
    }
    p_subpic = p_entry->p_subpic; /* get the local subpic */
    if( p_subpic->b_ephemer )
    {
        /* set a relativly long delay in hope that the next subtitle
           will arrive in this time and the real delay could be determined */
        p_subpic->i_stop = p_subpic->i_start + 20000000; /* start + 20 sec */
        p_subpic->b_ephemer = false;
    }
    SubsdelayEnforceDelayRules( p_filter );
    SubsdelayHeapUnlock( p_heap );
    return p_subpic;
}
