}
VdpStatus vdp_output_surface_query_put_bits_y_cb_cr_capabilities(
    const vdp_t *vdp, VdpDevice device,
    VdpRGBAFormat fmt, VdpYCbCrFormat yccfmt, VdpBool *ok)
{
    return vdp->vt.output_surface_query_put_bits_y_cb_cr_capabilities(device,
                                                              fmt, yccfmt, ok);
}
