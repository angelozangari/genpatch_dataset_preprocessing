vlc_module_end ()
static int RenderText( filter_t *p_filter, subpicture_region_t *p_region_out,
                       subpicture_region_t *p_region_in,
                       const vlc_fourcc_t *p_chroma_list )
{
    VLC_UNUSED(p_filter); VLC_UNUSED(p_region_out); VLC_UNUSED(p_region_in);
    VLC_UNUSED(p_chroma_list);
    return VLC_EGENERIC;
}
