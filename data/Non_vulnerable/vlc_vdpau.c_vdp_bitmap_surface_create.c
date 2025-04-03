}
VdpStatus vdp_bitmap_surface_create(const vdp_t *vdp, VdpDevice device,
    VdpRGBAFormat fmt, uint32_t w, uint32_t h, VdpBool fq,
    VdpBitmapSurface *surface)
{
    return vdp->vt.bitmap_surface_create(device, fmt, w, h, fq, surface);
}
