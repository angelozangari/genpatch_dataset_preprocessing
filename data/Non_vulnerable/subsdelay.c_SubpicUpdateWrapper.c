 *****************************************************************************/
static void SubpicUpdateWrapper( subpicture_t *p_subpic, const video_format_t *p_fmt_src,
                                  const video_format_t *p_fmt_dst, mtime_t i_ts )
{
    subsdelay_heap_entry_t *p_entry;
    mtime_t i_new_ts;
    p_entry = p_subpic->updater.p_sys;
    if( !p_entry )
    {
        return;
    }
    /* call source update */
    if( p_entry->p_source->updater.pf_update )
    {
        i_new_ts = p_entry->p_source->i_start +
                   ( (double)( p_entry->p_source->i_stop - p_entry->p_source->i_start ) * ( i_ts - p_entry->p_source->i_start ) ) /
                   ( p_entry->i_new_stop - p_entry->p_source->i_start );
        p_entry->p_source->p_region = p_entry->p_subpic->p_region;
        p_entry->p_source->updater.pf_update( p_entry->p_source, p_fmt_src, p_fmt_dst, i_new_ts );
        p_entry->p_subpic->p_region = p_entry->p_source->p_region;
    }
    SubpicLocalUpdate( p_subpic, i_ts );
}
