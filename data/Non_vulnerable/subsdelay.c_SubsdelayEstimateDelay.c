 *****************************************************************************/
static int64_t SubsdelayEstimateDelay( filter_t *p_filter, subsdelay_heap_entry_t *p_entry )
{
    int i_mode;
    int i_factor;
    int i_rank;
    i_mode = p_filter->p_sys->i_mode;
    i_factor = p_filter->p_sys->i_factor;
    if( i_mode == SUBSDELAY_MODE_ABSOLUTE )
    {
        return ( p_entry->p_source->i_stop - p_entry->p_source->i_start + INT_FACTOR_TO_MICROSEC( i_factor ) );
    }
    if( i_mode == SUBSDELAY_MODE_RELATIVE_SOURCE_CONTENT )
    {
        if( p_entry->p_subpic && p_entry->p_subpic->p_region && ( p_entry->p_subpic->p_region->psz_text
                || p_entry->p_subpic->p_region->psz_html ) )
        {
            if( p_entry->p_subpic->p_region->psz_text )
            {
                i_rank = SubsdelayGetTextRank( p_entry->p_subpic->p_region->psz_text );
            }
            else
            {
                i_rank = SubsdelayGetTextRank( p_entry->p_subpic->p_region->psz_html );
            }
            return ( i_rank * INT_FACTOR_TO_RANK_FACTOR( i_factor ) );
        }
        /* content is unavailable, calculation mode should be based on source delay */
        i_mode = SUBSDELAY_MODE_RELATIVE_SOURCE_DELAY;
    }
    if( i_mode == SUBSDELAY_MODE_RELATIVE_SOURCE_DELAY )
    {
        return ( ( i_factor * ( p_entry->p_source->i_stop - p_entry->p_source->i_start ) ) / INT_FACTOR_BASE );
    }
    return 10000000; /* 10 sec */
}
