}
VdpStatus vdp_output_surface_create(const vdp_t *vdp, VdpDevice device,
    VdpRGBAFormat fmt, uint32_t w, uint32_t h, VdpOutputSurface *surface)
{
    return vdp->vt.output_surface_create(device, fmt, w, h, surface);
}
