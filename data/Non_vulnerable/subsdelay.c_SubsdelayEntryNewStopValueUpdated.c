 *****************************************************************************/
static void SubsdelayEntryNewStopValueUpdated( subsdelay_heap_entry_t *p_entry )
{
    if( !p_entry->b_update_stop )
    {
        p_entry->p_subpic->i_stop = p_entry->i_new_stop - 100000; /* 0.1 sec less */
    }
}
