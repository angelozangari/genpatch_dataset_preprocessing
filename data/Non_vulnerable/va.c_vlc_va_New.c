}
vlc_va_t *vlc_va_New(vlc_object_t *obj, AVCodecContext *avctx,
                     const es_format_t *fmt)
{
    vlc_va_t *va = vlc_object_create(obj, sizeof (*va));
    if (unlikely(va == NULL))
        return NULL;
    va->module = vlc_module_load(va, "hw decoder", "$avcodec-hw", true,
                                 vlc_va_Start, va, avctx, fmt);
    if (va->module == NULL)
    {
        vlc_object_release(va);
        va = NULL;
    }
    return va;
}
