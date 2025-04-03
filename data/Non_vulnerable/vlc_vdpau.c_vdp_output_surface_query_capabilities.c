/*** Output surface ***/
VdpStatus vdp_output_surface_query_capabilities(const vdp_t *vdp,
    VdpDevice device, VdpRGBAFormat fmt, VdpBool *ok,
    uint32_t *max_width, uint32_t *max_height)
{
    return vdp->vt.output_surface_query_capabilities(device, fmt, ok,
                                                     max_width, max_height);
}
