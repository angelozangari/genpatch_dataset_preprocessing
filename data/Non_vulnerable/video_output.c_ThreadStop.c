}
static void ThreadStop(vout_thread_t *vout, vout_display_state_t *state)
{
    if (vout->p->spu_blend)
        filter_DeleteBlend(vout->p->spu_blend);
    /* Destroy translation tables */
    if (vout->p->display.vd) {
        if (vout->p->decoder_pool) {
            ThreadFlush(vout, true, INT64_MAX);
            vout_EndWrapper(vout);
        }
        vout_CloseWrapper(vout, state);
    }
    /* Destroy the video filters2 */
    filter_chain_Delete(vout->p->filter.chain_interactive);
    filter_chain_Delete(vout->p->filter.chain_static);
    video_format_Clean(&vout->p->filter.format);
    free(vout->p->filter.configuration);
    if (vout->p->decoder_fifo)
        picture_fifo_Delete(vout->p->decoder_fifo);
    assert(!vout->p->decoder_pool);
}
