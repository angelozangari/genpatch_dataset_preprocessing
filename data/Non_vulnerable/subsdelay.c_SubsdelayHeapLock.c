 *****************************************************************************/
static void SubsdelayHeapLock( subsdelay_heap_t *p_heap )
{
    vlc_mutex_lock( &p_heap->lock );
}
