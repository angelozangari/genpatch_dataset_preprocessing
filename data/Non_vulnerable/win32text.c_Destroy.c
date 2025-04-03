 *****************************************************************************/
static void Destroy( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    if( p_sys->hfont_bak ) SelectObject( p_sys->hcdc, p_sys->hfont_bak );
    if( p_sys->hfont ) DeleteObject( p_sys->hfont );
    DeleteDC( p_sys->hcdc );
    free( p_sys );
}
