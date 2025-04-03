}
VdpStatus vdp_output_surface_put_bits_native(const vdp_t *vdp,
    VdpOutputSurface surface, const void *const *data, uint32_t const *pitches,
    const VdpRect *dst)
{
    return vdp->vt.output_surface_put_bits_native(surface, data, pitches, dst);
}
