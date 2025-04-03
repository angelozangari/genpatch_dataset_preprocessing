}
static void Display(vout_display_t *vd, picture_t *picture, subpicture_t *subpicture)
{
    vout_display_sys_t *sys = vd->sys;
#define rect_src vd->sys->rect_src
#define rect_src_clipped vd->sys->rect_src_clipped
#define rect_dest vd->sys->rect_dest
#define rect_dest_clipped vd->sys->rect_dest_clipped
    RECT rect_dst = rect_dest_clipped;
    HDC hdc = GetDC(sys->hvideownd);
    OffsetRect(&rect_dst, -rect_dest.left, -rect_dest.top);
    SelectObject(sys->off_dc, sys->off_bitmap);
    if (rect_dest_clipped.right - rect_dest_clipped.left !=
        rect_src_clipped.right - rect_src_clipped.left ||
        rect_dest_clipped.bottom - rect_dest_clipped.top !=
        rect_src_clipped.bottom - rect_src_clipped.top) {
        StretchBlt(hdc, rect_dst.left, rect_dst.top,
                   rect_dst.right, rect_dst.bottom,
                   sys->off_dc,
                   rect_src_clipped.left,  rect_src_clipped.top,
                   rect_src_clipped.right, rect_src_clipped.bottom,
                   SRCCOPY);
    } else {
        BitBlt(hdc, rect_dst.left, rect_dst.top,
               rect_dst.right, rect_dst.bottom,
               sys->off_dc,
               rect_src_clipped.left, rect_src_clipped.top,
               SRCCOPY);
    }
    ReleaseDC(sys->hvideownd, hdc);
#undef rect_src
#undef rect_src_clipped
#undef rect_dest
#undef rect_dest_clipped
    /* TODO */
    picture_Release(picture);
    VLC_UNUSED(subpicture);
    CommonDisplay(vd);
}
