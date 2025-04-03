 *****************************************************************************/
static int Create( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    const vlc_chroma_description_t *p_chroma =
        vlc_fourcc_GetChromaDescription( p_filter->fmt_in.video.i_chroma );
    if( p_chroma == NULL || p_chroma->plane_count == 0 )
        return VLC_EGENERIC;
    /* Allocate structure */
    p_filter->p_sys = malloc( sizeof( filter_sys_t ) );
    if( p_filter->p_sys == NULL )
        return VLC_ENOMEM;
    p_filter->pf_video_filter = Filter;
    p_filter->p_sys->f_angle = 0.0;
    p_filter->p_sys->last_date = 0;
    return VLC_SUCCESS;
}
