}
static filter_t *SpuRenderCreateAndLoadText(spu_t *spu)
{
    filter_t *text = vlc_custom_create(spu, sizeof(*text), "spu text");
    if (!text)
        return NULL;
    text->p_owner = xmalloc(sizeof(*text->p_owner));
    text->p_owner->spu = spu;
    es_format_Init(&text->fmt_in, VIDEO_ES, 0);
    es_format_Init(&text->fmt_out, VIDEO_ES, 0);
    text->fmt_out.video.i_width          =
    text->fmt_out.video.i_visible_width  = 32;
    text->fmt_out.video.i_height         =
    text->fmt_out.video.i_visible_height = 32;
    text->pf_get_attachments = spu_get_attachments;
    text->p_module = module_need(text, "text renderer", "$text-renderer", false);
    /* Create a few variables used for enhanced text rendering */
    var_Create(text, "spu-elapsed",   VLC_VAR_TIME);
    var_Create(text, "text-rerender", VLC_VAR_BOOL);
    return text;
}
