}
VdpStatus vdp_output_surface_put_bits_indexed(const vdp_t *vdp,
    VdpOutputSurface surface, VdpIndexedFormat fmt, const void *const *data,
    const uint32_t *pitch, const VdpRect *dst,
    VdpColorTableFormat tabfmt, const void *tab)
{
    return vdp->vt.output_surface_put_bits_indexed(surface, fmt, data, pitch,
                                                   dst, tabfmt, tab);
}
