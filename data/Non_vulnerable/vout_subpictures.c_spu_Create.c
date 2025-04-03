 */
spu_t *spu_Create(vlc_object_t *object)
{
    spu_t *spu = vlc_custom_create(object,
                                   sizeof(spu_t) + sizeof(spu_private_t),
                                   "subpicture");
    if (!spu)
        return NULL;
    /* Initialize spu fields */
    spu_private_t *sys = spu->p = (spu_private_t*)&spu[1];
    /* Initialize private fields */
    vlc_mutex_init(&sys->lock);
    SpuHeapInit(&sys->heap);
    sys->text = NULL;
    sys->scale = NULL;
    sys->scale_yuvp = NULL;
    sys->margin = var_InheritInteger(spu, "sub-margin");
    /* Register the default subpicture channel */
    sys->channel = SPU_DEFAULT_CHANNEL + 1;
    sys->source_chain_update = NULL;
    sys->filter_chain_update = NULL;
    vlc_mutex_init(&sys->source_chain_lock);
    vlc_mutex_init(&sys->filter_chain_lock);
    sys->source_chain = filter_chain_New(spu, "sub source", false,
                                         SubSourceAllocationInit,
                                         SubSourceAllocationClean,
                                         spu);
    sys->filter_chain = filter_chain_New(spu, "sub filter", false,
                                         NULL,
                                         NULL,
                                         spu);
    /* Load text and scale module */
    sys->text = SpuRenderCreateAndLoadText(spu);
    /* XXX spu->p_scale is used for all conversion/scaling except yuvp to
     * yuva/rgba */
    sys->scale = SpuRenderCreateAndLoadScale(VLC_OBJECT(spu),
                                             VLC_CODEC_YUVA, VLC_CODEC_RGBA, true);
    /* This one is used for YUVP to YUVA/RGBA without scaling
     * FIXME rename it */
    sys->scale_yuvp = SpuRenderCreateAndLoadScale(VLC_OBJECT(spu),
                                                  VLC_CODEC_YUVP, VLC_CODEC_YUVA, false);
    /* */
    sys->last_sort_date = -1;
    return spu;
}
