 *****************************************************************************/
static void SubsdelayEntryDestroy( subsdelay_heap_entry_t *p_entry )
{
    SubpicDestroyClone( p_entry->p_source );
    free( p_entry );
}
