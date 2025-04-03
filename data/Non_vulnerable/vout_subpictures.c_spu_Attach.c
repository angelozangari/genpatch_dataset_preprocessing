 */
void spu_Attach(spu_t *spu, vlc_object_t *input, bool attach)
{
    if (attach) {
        UpdateSPU(spu, input);
        var_Create(input, "highlight", VLC_VAR_BOOL);
        var_AddCallback(input, "highlight", CropCallback, spu);
        vlc_mutex_lock(&spu->p->lock);
        spu->p->input = input;
        if (spu->p->text)
            FilterRelease(spu->p->text);
        spu->p->text = SpuRenderCreateAndLoadText(spu);
        vlc_mutex_unlock(&spu->p->lock);
    } else {
        vlc_mutex_lock(&spu->p->lock);
        spu->p->input = NULL;
        vlc_mutex_unlock(&spu->p->lock);
        /* Delete callbacks */
        var_DelCallback(input, "highlight", CropCallback, spu);
        var_Destroy(input, "highlight");
    }
}
