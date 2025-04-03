 *****************************************************************************/
static subsdelay_heap_entry_t * SubsdelayEntryCreate( subpicture_t *p_source, filter_t *p_filter )
{
    subsdelay_heap_entry_t *p_entry;
    subpicture_t *p_new_subpic;
    subpicture_updater_t updater;
    /* allocate structure */
    p_entry = (subsdelay_heap_entry_t *) malloc( sizeof( subsdelay_heap_entry_t ) );
    if( !p_entry )
    {
        return NULL;
    }
    /* initialize local updater */
    updater.p_sys = p_entry;
    updater.pf_validate = SubpicValidateWrapper;
    updater.pf_update = SubpicUpdateWrapper;
    updater.pf_destroy = SubpicDestroyWrapper;
    /* create new subpic */
    p_new_subpic = SubpicClone( p_source,  &updater );
    if( !p_new_subpic )
    {
        free( p_entry );
        return NULL;
    }
    /* initialize entry */
    p_entry->p_subpic = p_new_subpic;
    p_entry->p_source = p_source;
    p_entry->p_filter = p_filter;
    p_entry->p_next = NULL;
    p_entry->b_update_stop = true;
    p_entry->b_update_ephemer = p_source->b_ephemer;
    p_entry->b_update_position = true;
    p_entry->b_check_empty = true;
    p_entry->i_new_stop = p_source->i_stop;
    p_entry->b_last_region_saved = false;
    p_entry->i_last_region_x = 0;
    p_entry->i_last_region_y = 0;
    p_entry->i_last_region_align = 0;
    return p_entry;
}
