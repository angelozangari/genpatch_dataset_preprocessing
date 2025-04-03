 */
void spu_Destroy(spu_t *spu)
{
    spu_private_t *sys = spu->p;
    if (sys->text)
        FilterRelease(sys->text);
    if (sys->scale_yuvp)
        FilterRelease(sys->scale_yuvp);
    if (sys->scale)
        FilterRelease(sys->scale);
    filter_chain_Delete(sys->source_chain);
    filter_chain_Delete(sys->filter_chain);
    vlc_mutex_destroy(&sys->source_chain_lock);
    vlc_mutex_destroy(&sys->filter_chain_lock);
    free(sys->source_chain_update);
    free(sys->filter_chain_update);
    /* Destroy all remaining subpictures */
    SpuHeapClean(&sys->heap);
    vlc_mutex_destroy(&sys->lock);
    vlc_object_release(spu);
}
