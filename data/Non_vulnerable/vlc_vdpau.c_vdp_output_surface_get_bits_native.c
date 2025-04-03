}
VdpStatus vdp_output_surface_get_bits_native(const vdp_t *vdp,
    VdpOutputSurface surface, const VdpRect *src,
    void *const *data, uint32_t const *pitches)
{
    return vdp->vt.output_surface_get_bits_native(surface, src, data, pitches);
}
