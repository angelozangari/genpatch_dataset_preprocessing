 *****************************************************************************/
static void SubsdelayHeapInit( subsdelay_heap_t *p_heap )
{
    p_heap->i_count = 0;
    p_heap->p_head = NULL;
    vlc_mutex_init( &p_heap->lock );
}
