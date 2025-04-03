}
VdpStatus vdp_video_surface_get_bits_y_cb_cr(const vdp_t *vdp,
    VdpVideoSurface surface, VdpYCbCrFormat fmt,
    void *const *data, uint32_t const *pitches)
{
    return vdp->vt.video_surface_get_bits_y_cb_cr(surface, fmt, data, pitches);
}
