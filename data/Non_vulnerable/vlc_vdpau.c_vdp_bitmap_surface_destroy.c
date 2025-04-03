}
VdpStatus vdp_bitmap_surface_destroy(const vdp_t *vdp,
                                     VdpBitmapSurface surface)
{
    return vdp->vt.bitmap_surface_destroy(surface);
}
