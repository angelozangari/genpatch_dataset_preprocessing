}
VdpStatus vdp_output_surface_get_parameters(const vdp_t *vdp,
    VdpOutputSurface surface, VdpRGBAFormat *fmt, uint32_t *w, uint32_t *h)
{
    return vdp->vt.output_surface_get_parameters(surface, fmt, w, h);
}
