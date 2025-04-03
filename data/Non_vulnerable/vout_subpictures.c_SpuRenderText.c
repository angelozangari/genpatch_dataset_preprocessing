}
static void SpuRenderText(spu_t *spu, bool *rerender_text,
                          subpicture_region_t *region,
                          const vlc_fourcc_t *chroma_list,
                          mtime_t elapsed_time)
{
    filter_t *text = spu->p->text;
    assert(region->fmt.i_chroma == VLC_CODEC_TEXT);
    if (!text || !text->p_module)
        return;
    /* Setup 3 variables which can be used to render
     * time-dependent text (and effects). The first indicates
     * the total amount of time the text will be on screen,
     * the second the amount of time it has already been on
     * screen (can be a negative value as text is layed out
     * before it is rendered) and the third is a feedback
     * variable from the renderer - if the renderer sets it
     * then this particular text is time-dependent, eg. the
     * visual progress bar inside the text in karaoke and the
     * text needs to be rendered multiple times in order for
     * the effect to work - we therefore need to return the
     * region to its original state at the end of the loop,
     * instead of leaving it in YUVA or YUVP.
     * Any renderer which is unaware of how to render
     * time-dependent text can happily ignore the variables
     * and render the text the same as usual - it should at
     * least show up on screen, but the effect won't change
     * the text over time.
     */
    var_SetTime(text, "spu-elapsed", elapsed_time);
    var_SetBool(text, "text-rerender", false);
    if (text->pf_render_html && region->psz_html)
        text->pf_render_html(text, region, region, chroma_list);
    else if (text->pf_render_text)
        text->pf_render_text(text, region, region, chroma_list);
    *rerender_text = var_GetBool(text, "text-rerender");
}
