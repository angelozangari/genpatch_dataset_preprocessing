}
static int Init(vout_display_t *vd,
                video_format_t *fmt, int width, int height)
{
    vout_display_sys_t *sys = vd->sys;
    /* */
    RECT *display = &sys->rect_display;
    display->left   = 0;
    display->top    = 0;
    display->right  = GetSystemMetrics(SM_CXSCREEN);;
    display->bottom = GetSystemMetrics(SM_CYSCREEN);;
    /* Initialize an offscreen bitmap for direct buffer operations. */
    /* */
    HDC window_dc = GetDC(sys->hvideownd);
    /* */
    sys->i_depth = GetDeviceCaps(window_dc, PLANES) *
                   GetDeviceCaps(window_dc, BITSPIXEL);
    /* */
    msg_Dbg(vd, "GDI depth is %i", sys->i_depth);
    switch (sys->i_depth) {
    case 8:
        fmt->i_chroma = VLC_CODEC_RGB8;
        break;
    case 15:
        fmt->i_chroma = VLC_CODEC_RGB15;
        fmt->i_rmask  = 0x7c00;
        fmt->i_gmask  = 0x03e0;
        fmt->i_bmask  = 0x001f;
        break;
    case 16:
        fmt->i_chroma = VLC_CODEC_RGB16;
        fmt->i_rmask  = 0xf800;
        fmt->i_gmask  = 0x07e0;
        fmt->i_bmask  = 0x001f;
        break;
    case 24:
        fmt->i_chroma = VLC_CODEC_RGB24;
        fmt->i_rmask  = 0x00ff0000;
        fmt->i_gmask  = 0x0000ff00;
        fmt->i_bmask  = 0x000000ff;
        break;
    case 32:
        fmt->i_chroma = VLC_CODEC_RGB32;
        fmt->i_rmask  = 0x00ff0000;
        fmt->i_gmask  = 0x0000ff00;
        fmt->i_bmask  = 0x000000ff;
        break;
    default:
        msg_Err(vd, "screen depth %i not supported", sys->i_depth);
        return VLC_EGENERIC;
    }
    fmt->i_width  = width;
    fmt->i_height = height;
    void *p_pic_buffer;
    int     i_pic_pitch;
    /* Initialize offscreen bitmap */
    BITMAPINFO *bi = &sys->bitmapinfo;
    memset(bi, 0, sizeof(BITMAPINFO) + 3 * sizeof(RGBQUAD));
    if (sys->i_depth > 8) {
        ((DWORD*)bi->bmiColors)[0] = fmt->i_rmask;
        ((DWORD*)bi->bmiColors)[1] = fmt->i_gmask;
        ((DWORD*)bi->bmiColors)[2] = fmt->i_bmask;;
    }
    BITMAPINFOHEADER *bih = &sys->bitmapinfo.bmiHeader;
    bih->biSize = sizeof(BITMAPINFOHEADER);
    bih->biSizeImage     = 0;
    bih->biPlanes        = 1;
    bih->biCompression   = (sys->i_depth == 15 ||
                            sys->i_depth == 16) ? BI_BITFIELDS : BI_RGB;
    bih->biBitCount      = sys->i_depth;
    bih->biWidth         = fmt->i_width;
    bih->biHeight        = -fmt->i_height;
    bih->biClrImportant  = 0;
    bih->biClrUsed       = 0;
    bih->biXPelsPerMeter = 0;
    bih->biYPelsPerMeter = 0;
    i_pic_pitch = bih->biBitCount * bih->biWidth / 8;
    sys->off_bitmap = CreateDIBSection(window_dc,
                                       (BITMAPINFO *)bih,
                                       DIB_RGB_COLORS,
                                       &p_pic_buffer, NULL, 0);
    sys->off_dc = CreateCompatibleDC(window_dc);
    SelectObject(sys->off_dc, sys->off_bitmap);
    ReleaseDC(sys->hvideownd, window_dc);
    EventThreadUpdateTitle(sys->event, VOUT_TITLE " (WinGDI output)");
    /* */
    picture_resource_t rsc;
    memset(&rsc, 0, sizeof(rsc));
    rsc.p[0].p_pixels = p_pic_buffer;
    rsc.p[0].i_lines  = fmt->i_height;
    rsc.p[0].i_pitch  = i_pic_pitch;;
    picture_t *picture = picture_NewFromResource(fmt, &rsc);
    if (picture) {
        picture_pool_configuration_t cfg;
        memset(&cfg, 0, sizeof(cfg));
        cfg.picture_count = 1;
        cfg.picture = &picture;
        sys->pool = picture_pool_NewExtended(&cfg);
    } else
        sys->pool = NULL;
    UpdateRects(vd, NULL, NULL, true);
    return VLC_SUCCESS;
}
