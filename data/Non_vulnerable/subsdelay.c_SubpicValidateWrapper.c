 *****************************************************************************/
static int SubpicValidateWrapper( subpicture_t *p_subpic, bool has_src_changed, const video_format_t *p_fmt_src,
                                  bool has_dst_changed, const video_format_t *p_fmt_dst, mtime_t i_ts )
{
    subsdelay_heap_entry_t *p_entry;
    mtime_t i_new_ts;
    int i_result = VLC_SUCCESS;
    p_entry = p_subpic->updater.p_sys;
    if( !p_entry )
    {
        return VLC_SUCCESS;
    }
    /* call source validate */
    if( p_entry->p_source->updater.pf_validate )
    {
        i_new_ts = p_entry->p_source->i_start +
                   ( (double)( p_entry->p_source->i_stop - p_entry->p_source->i_start ) * ( i_ts - p_entry->p_source->i_start ) ) /
                   ( p_entry->i_new_stop - p_entry->p_source->i_start );
        i_result = p_entry->p_source->updater.pf_validate( p_entry->p_source, has_src_changed, p_fmt_src,
                                                        has_dst_changed, p_fmt_dst, i_new_ts );
    }
    p_entry->b_last_region_saved = false;
    if( p_subpic->p_region )
    {
        /* save copy */
        p_entry->i_last_region_x = p_subpic->p_region->i_x;
        p_entry->i_last_region_y = p_subpic->p_region->i_y;
        p_entry->i_last_region_align = p_subpic->p_region->i_align;
        p_entry->b_last_region_saved = true;
    }
    if( !i_result )
    {
        /* subpic update isn't necessary, so local update should be called here */
        SubpicLocalUpdate( p_subpic, i_ts );
    }
    return i_result;
}
