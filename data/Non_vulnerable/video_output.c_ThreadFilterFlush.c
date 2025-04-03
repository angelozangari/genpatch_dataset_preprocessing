}
static void ThreadFilterFlush(vout_thread_t *vout, bool is_locked)
{
    if (vout->p->displayed.current)
        picture_Release( vout->p->displayed.current );
    vout->p->displayed.current = NULL;
    if (vout->p->displayed.next)
        picture_Release( vout->p->displayed.next );
    vout->p->displayed.next = NULL;
    if (!is_locked)
        vlc_mutex_lock(&vout->p->filter.lock);
    filter_chain_VideoFlush(vout->p->filter.chain_static);
    filter_chain_VideoFlush(vout->p->filter.chain_interactive);
    if (!is_locked)
        vlc_mutex_unlock(&vout->p->filter.lock);
}
