}
static void Plane_VFlip(plane_t *restrict dst, const plane_t *restrict src)
{
    const uint8_t *src_pixels = src->p_pixels;
    uint8_t *restrict dst_pixels = dst->p_pixels;
    dst_pixels += dst->i_pitch * dst->i_visible_lines;
    for (int y = 0; y < dst->i_visible_lines; y++) {
        dst_pixels -= dst->i_pitch;
        memcpy(dst_pixels, src_pixels, dst->i_visible_pitch);
        src_pixels += src->i_pitch;
    }
}
