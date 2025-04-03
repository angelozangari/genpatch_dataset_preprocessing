 */
int spu_ProcessMouse(spu_t *spu,
                     const vlc_mouse_t *mouse,
                     const video_format_t *fmt)
{
    spu_private_t *sys = spu->p;
    vlc_mutex_lock(&sys->source_chain_lock);
    filter_chain_MouseEvent(sys->source_chain, mouse, fmt);
    vlc_mutex_unlock(&sys->source_chain_lock);
    return VLC_SUCCESS;
}
