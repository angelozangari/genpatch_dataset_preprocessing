 *****************************************************************************/
static void SubsdelayEnforceDelayRules( filter_t *p_filter )
{
    subsdelay_heap_entry_t ** p_list;
    int i, j, i_count, i_overlap;
    int64_t i_offset;
    int64_t i_min_stops_interval;
    int64_t i_min_stop_start_interval;
    int64_t i_min_start_stop_interval;
    p_list = p_filter->p_sys->heap.p_list;
    i_count = p_filter->p_sys->heap.i_count;
    i_overlap = p_filter->p_sys->i_overlap;
    i_min_stops_interval = p_filter->p_sys->i_min_stops_interval;
    i_min_stop_start_interval = p_filter->p_sys->i_min_stop_start_interval;
    i_min_start_stop_interval = p_filter->p_sys->i_min_start_stop_interval;
    /* step 1 - enforce min stops interval rule (extend delays) */
    /* look for:
    [subtitle 1 ..............]
           [subtitle 2 ..............]
                              |<-MinStopsInterval->|
     * and extend newer subtitle:
    [subtitle 1 ..............]
           [subtitle 2 ............................]
                              |<-MinStopsInterval->|
    */
    for( i = 0; i < i_count - 1; i++ )
    {
        p_list[i + 1]->i_new_stop = __MAX( p_list[i + 1]->i_new_stop,
                p_list[i]->i_new_stop + i_min_stops_interval );
    }
    /* step 2 - enforce min stop start interval rule (extend delays) */
    /* look for:
    [subtitle 1 .........]
                                   [subtitle 2 ....]
          |<-MinStopStartInterval->|
     * and fill the gap:
    [subtitle 1 ..................]
                                   [subtitle 2 ....]
          |<-MinStopStartInterval->|
    */
    for( i = 0; i < i_count; i++ )
    {
        for( j = i + 1; j < __MIN( i_count, i + 1 + i_overlap ); j++ )
        {
            i_offset = p_list[j]->p_source->i_start - p_list[i]->i_new_stop;
            if( i_offset <= 0 )
            {
                continue;
            }
            if( i_offset < i_min_stop_start_interval )
            {
                p_list[i]->i_new_stop = p_list[j]->p_source->i_start;
            }
            break;
        }
    }
    /* step 3 - enforce min start stop interval rule (shorten delays) */
    /* look for:
    [subtitle 1 ............]
                    [subtitle 2 ....................]
                    |<-MinStartStopInterval->|
     * and remove the overlapping part:
    [subtitle 1 ...]
                    [subtitle 2 ....................]
                    |<-MinStartStopInterval->|
    */
    for( i = 0; i < i_count; i++ )
    {
        for( j = i + 1; j < __MIN( i_count, i + 1 + i_overlap ); j++ )
        {
            i_offset = p_list[i]->i_new_stop - p_list[j]->p_source->i_start;
            if( i_offset <= 0 )
            {
                break;
            }
            if( i_offset < i_min_start_stop_interval )
            {
                p_list[i]->i_new_stop = p_list[j]->p_source->i_start;
                break;
            }
        }
    }
    /* step 4 - enforce max overlapping rule (shorten delays)*/
    /* look for: (overlap = 2)
    [subtitle 1 ..............]
             [subtitle 2 ..............]
                      [subtitle 3 ..............]
     * and cut older subtitle:
    [subtitle 1 .....]
             [subtitle 2 ..............]
                      [subtitle 3 ..............]
    */
    for( i = 0; i < i_count - i_overlap; i++ )
    {
        if( p_list[i]->i_new_stop > p_list[i + i_overlap]->p_source->i_start )
        {
            p_list[i]->i_new_stop = p_list[i + i_overlap]->p_source->i_start;
        }
    }
    /* finally - update all */
    for( i = 0; i < i_count; i++ )
    {
        SubsdelayEntryNewStopValueUpdated( p_list[i] );
    }
}
