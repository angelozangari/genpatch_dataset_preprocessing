}
VdpStatus vdp_bitmap_surface_get_parameters(const vdp_t *vdp,
    VdpBitmapSurface surface, VdpRGBAFormat *fmt, uint32_t *w, uint32_t *h,
    VdpBool *fq)
{
    return vdp->vt.bitmap_surface_get_parameters(surface, fmt, w, h, fq);
}
