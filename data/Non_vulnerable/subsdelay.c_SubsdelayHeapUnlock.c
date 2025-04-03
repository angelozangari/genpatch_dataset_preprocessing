 *****************************************************************************/
static void SubsdelayHeapUnlock( subsdelay_heap_t *p_heap )
{
    vlc_mutex_unlock( &p_heap->lock );
}
