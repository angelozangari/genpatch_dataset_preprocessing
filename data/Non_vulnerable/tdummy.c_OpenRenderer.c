}
static int OpenRenderer( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    p_filter->pf_render_text = RenderText;
    p_filter->pf_render_html = NULL;
    return VLC_SUCCESS;
}
