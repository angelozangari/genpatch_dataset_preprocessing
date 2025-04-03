}
static int SetFont( filter_t *p_filter, int i_size )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    LOGFONT logfont;
    if( i_size && i_size == p_sys->i_font_size ) return VLC_SUCCESS;
    if( !i_size )
    {
        vlc_value_t val;
        if( !p_sys->i_default_font_size &&
            p_sys->i_display_height == (int)p_filter->fmt_out.video.i_height )
            return VLC_SUCCESS;
        if( p_sys->i_default_font_size )
        {
            i_size = p_sys->i_default_font_size;
        }
        else
        {
            var_Get( p_filter, "win32text-rel-fontsize", &val );
            i_size = (int)p_filter->fmt_out.video.i_height / val.i_int;
            p_filter->p_sys->i_display_height =
                p_filter->fmt_out.video.i_height;
        }
        if( i_size <= 0 )
        {
            msg_Warn( p_filter, "invalid fontsize, using 12" );
            i_size = 12;
        }
        msg_Dbg( p_filter, "using fontsize: %i", i_size );
    }
    p_sys->i_font_size = i_size;
    if( p_sys->hfont_bak ) SelectObject( p_sys->hcdc, p_sys->hfont_bak );
    if( p_sys->hfont ) DeleteObject( p_sys->hfont );
    i_size = i_size * (int64_t)p_sys->i_logpy / 72;
    logfont.lfHeight = i_size;
    logfont.lfWidth = 0;
    logfont.lfEscapement = 0;
    logfont.lfOrientation = 0;
    logfont.lfWeight = 0;
    logfont.lfItalic = FALSE;
    logfont.lfUnderline = FALSE;
    logfont.lfStrikeOut = FALSE;
    logfont.lfCharSet = ANSI_CHARSET;
    logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
    logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    logfont.lfQuality = ANTIALIASED_QUALITY;
    logfont.lfPitchAndFamily = DEFAULT_PITCH;
    memcpy( logfont.lfFaceName, _T("Arial"), sizeof(_T("Arial")) );
    p_sys->hfont = CreateFontIndirect( &logfont );
    p_sys->hfont_bak = SelectObject( p_sys->hcdc, p_sys->hfont );
    return VLC_SUCCESS;
}
