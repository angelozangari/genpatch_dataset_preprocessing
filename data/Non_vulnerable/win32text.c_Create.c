 *****************************************************************************/
static int Create( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys;
    char *psz_fontfile = NULL;
    vlc_value_t val;
    HDC hdc;
    /* Allocate structure */
    p_filter->p_sys = p_sys = malloc( sizeof( filter_sys_t ) );
    if( !p_sys )
        return VLC_ENOMEM;
    p_sys->i_font_size = 0;
    p_sys->i_display_height = 0;
    var_Create( p_filter, "win32text-font",
                VLC_VAR_STRING | VLC_VAR_DOINHERIT );
    var_Create( p_filter, "win32text-fontsize",
                VLC_VAR_INTEGER | VLC_VAR_DOINHERIT );
    var_Create( p_filter, "win32text-rel-fontsize",
                VLC_VAR_INTEGER | VLC_VAR_DOINHERIT );
    var_Create( p_filter, "win32text-opacity",
                VLC_VAR_INTEGER | VLC_VAR_DOINHERIT );
    var_Get( p_filter, "win32text-opacity", &val );
    p_sys->i_font_opacity = VLC_CLIP( val.i_int, 0, 255 );
    var_Create( p_filter, "win32text-color",
                VLC_VAR_INTEGER | VLC_VAR_DOINHERIT );
    var_Get( p_filter, "win32text-color", &val );
    p_sys->i_font_color = VLC_CLIP( val.i_int, 0, 0xFFFFFF );
    p_sys->hfont = p_sys->hfont_bak = 0;
    hdc = GetDC( NULL );
    p_sys->hcdc = CreateCompatibleDC( hdc );
    p_sys->i_logpy = GetDeviceCaps( hdc, LOGPIXELSY );
    ReleaseDC( NULL, hdc );
    SetBkMode( p_sys->hcdc, TRANSPARENT );
    var_Get( p_filter, "win32text-fontsize", &val );
    p_sys->i_default_font_size = val.i_int;
    if( SetFont( p_filter, 0 ) != VLC_SUCCESS ) goto error;
    free( psz_fontfile );
    p_filter->pf_render_text = RenderText;
    p_filter->pf_render_html = NULL;
    return VLC_SUCCESS;
 error:
    free( psz_fontfile );
    free( p_sys );
    return VLC_EGENERIC;
}
