}
VdpStatus vdp_output_surface_put_bits_y_cb_cr(const vdp_t *vdp,
    VdpOutputSurface surface, VdpYCbCrFormat fmt, const void *const *data,
    const uint32_t *pitches, const VdpRect *dst, const VdpCSCMatrix *mtx)
{
    return vdp->vt.output_surface_put_bits_y_cb_cr(surface, fmt, data,
                                                   pitches, dst, mtx);
}
