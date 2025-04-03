}
VdpStatus vdp_output_surface_query_put_bits_indexed_capabilities(
    const vdp_t *vdp, VdpDevice device, VdpRGBAFormat fmt,
    VdpIndexedFormat idxfmt, VdpColorTableFormat colfmt, VdpBool *ok)
{
    return vdp->vt.output_surface_query_put_bits_indexed_capabilities(device,
                                                      fmt, idxfmt, colfmt, ok);
}
