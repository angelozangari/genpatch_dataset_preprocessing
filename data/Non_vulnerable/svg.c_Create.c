 *****************************************************************************/
static int Create( vlc_object_t *p_this )
{
    filter_t *p_filter = ( filter_t * )p_this;
    filter_sys_t *p_sys;
    /* Allocate structure */
    p_sys = malloc( sizeof( filter_sys_t ) );
    if( !p_sys )
        return VLC_ENOMEM;
    /* Initialize psz_template */
    p_sys->psz_template = svg_GetTemplate( p_this );
    if( !p_sys->psz_template )
    {
        free( p_sys );
        return VLC_ENOMEM;
    }
    p_sys->i_width = p_filter->fmt_out.video.i_width;
    p_sys->i_height = p_filter->fmt_out.video.i_height;
    p_filter->pf_render_text = RenderText;
    p_filter->pf_render_html = NULL;
    p_filter->p_sys = p_sys;
    /* MUST call this before any RSVG funcs */
    rsvg_init( );
    return VLC_SUCCESS;
}
