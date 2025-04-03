}
VdpStatus vdp_bitmap_surface_put_bits_native(const vdp_t *vdp,
    VdpBitmapSurface surface, const void *const *data, const uint32_t *pitch,
    const VdpRect *rect)
{
    return vdp->vt.bitmap_surface_put_bits_native(surface, data, pitch, rect);
}
