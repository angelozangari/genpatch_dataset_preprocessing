}
void subpicture_Update( subpicture_t *p_subpicture,
                        const video_format_t *p_fmt_src,
                        const video_format_t *p_fmt_dst,
                        mtime_t i_ts )
{
    subpicture_updater_t *p_upd = &p_subpicture->updater;
    subpicture_private_t *p_private = p_subpicture->p_private;
    if( !p_upd->pf_validate )
        return;
    if( !p_upd->pf_validate( p_subpicture,
                          !video_format_IsSimilar( p_fmt_src,
                                                   &p_private->src ), p_fmt_src,
                          !video_format_IsSimilar( p_fmt_dst,
                                                   &p_private->dst ), p_fmt_dst,
                          i_ts ) )
        return;
    subpicture_region_ChainDelete( p_subpicture->p_region );
    p_subpicture->p_region = NULL;
    p_upd->pf_update( p_subpicture, p_fmt_src, p_fmt_dst, i_ts );
    video_format_Clean( &p_private->src );
    video_format_Clean( &p_private->dst );
    video_format_Copy( &p_private->src, p_fmt_src );
    video_format_Copy( &p_private->dst, p_fmt_dst );
}
