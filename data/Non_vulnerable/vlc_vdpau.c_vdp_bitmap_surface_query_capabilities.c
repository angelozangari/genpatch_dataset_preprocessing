/*** Bitmap surface ***/
VdpStatus vdp_bitmap_surface_query_capabilities(const vdp_t *vdp,
    VdpDevice device, VdpRGBAFormat fmt, VdpBool *ok, uint32_t *w, uint32_t *h)
{
    return vdp->vt.bitmap_surface_query_capabilities(device, fmt, ok, w, h);
}
