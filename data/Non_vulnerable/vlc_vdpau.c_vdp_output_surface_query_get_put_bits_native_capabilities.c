}
VdpStatus vdp_output_surface_query_get_put_bits_native_capabilities(
    const vdp_t *vdp, VdpDevice device, VdpRGBAFormat fmt, VdpBool *ok)
{
    return vdp->vt.output_surface_query_get_put_bits_native_capabilities(
                                                              device, fmt, ok);
}
