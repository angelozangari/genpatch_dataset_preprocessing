}
static int RenderText( filter_t *p_filter, subpicture_region_t *p_region_out,
                       subpicture_region_t *p_region_in,
                       const vlc_fourcc_t *p_chroma_list )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    int i_font_color, i_font_alpha, i_font_size;
    uint8_t *p_bitmap;
    TCHAR *psz_string;
    int i, i_width, i_height;
    HBITMAP bitmap, bitmap_bak;
    BITMAPINFO *p_bmi;
    RECT rect = { 0, 0, 0, 0 };
    /* Sanity check */
    if( !p_region_in || !p_region_out ) return VLC_EGENERIC;
    if( !p_region_in->psz_text || !*p_region_in->psz_text )
        return VLC_EGENERIC;
    psz_string = ToT(p_region_in->psz_text);
    if( psz_string == NULL )
        return VLC_EGENERIC;
    if( !*psz_string )
    {
        free( psz_string );
        return VLC_EGENERIC;
    }
    if( p_region_in->p_style )
    {
        i_font_color = VLC_CLIP( p_region_in->p_style->i_font_color, 0, 0xFFFFFF );
        i_font_alpha = VLC_CLIP( p_region_in->p_style->i_font_alpha, 0, 255 );
        i_font_size  = VLC_CLIP( p_region_in->p_style->i_font_size, 0, 255 );
    }
    else
    {
        i_font_color = p_sys->i_font_color;
        i_font_alpha = 255 - p_sys->i_font_opacity;
        i_font_size = p_sys->i_default_font_size;
    }
    SetFont( p_filter, i_font_size );
    SetTextColor( p_sys->hcdc, RGB( (i_font_color >> 16) & 0xff,
                  (i_font_color >> 8) & 0xff, i_font_color & 0xff) );
    DrawText( p_sys->hcdc, psz_string, -1, &rect,
              DT_CALCRECT | DT_CENTER | DT_NOPREFIX );
    i_width = rect.right; i_height = rect.bottom;
    p_bmi = malloc(sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*16);
    memset( p_bmi, 0, sizeof(BITMAPINFOHEADER) );
    p_bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    p_bmi->bmiHeader.biWidth = (i_width+3) & ~3;
    p_bmi->bmiHeader.biHeight = - i_height;
    p_bmi->bmiHeader.biPlanes = 1;
    p_bmi->bmiHeader.biBitCount = 8;
    p_bmi->bmiHeader.biCompression = BI_RGB;
    p_bmi->bmiHeader.biClrUsed = 16;
    for( i = 0; i < 16; i++ )
    {
        p_bmi->bmiColors[i].rgbBlue =
            p_bmi->bmiColors[i].rgbGreen =
                p_bmi->bmiColors[i].rgbRed = pi_gamma[i];
    }
    bitmap = CreateDIBSection( p_sys->hcdc, p_bmi, DIB_RGB_COLORS,
                               (void **)&p_bitmap, NULL, 0 );
    if( !bitmap )
    {
        msg_Err( p_filter, "could not create bitmap" );
        free( psz_string );
        return VLC_EGENERIC;
    }
    bitmap_bak = SelectObject( p_sys->hcdc, bitmap );
    FillRect( p_sys->hcdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH) );
    if( !DrawText( p_sys->hcdc, psz_string, -1, &rect,
                   DT_CENTER | DT_NOPREFIX ) )
    {
        msg_Err( p_filter, "could not draw text" );
    }
    p_region_out->i_x = p_region_in->i_x;
    p_region_out->i_y = p_region_in->i_y;
    Render( p_filter, p_region_out, p_bitmap, i_width, i_height );
    SelectObject( p_sys->hcdc, bitmap_bak );
    DeleteObject( bitmap );
    free( psz_string );
    return VLC_SUCCESS;
}
