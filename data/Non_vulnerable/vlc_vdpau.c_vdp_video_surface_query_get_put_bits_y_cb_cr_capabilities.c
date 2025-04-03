}
VdpStatus vdp_video_surface_query_get_put_bits_y_cb_cr_capabilities(
    const vdp_t *vdp, VdpDevice device, VdpChromaType type, VdpYCbCrFormat fmt,
    VdpBool *ok)
{
    return vdp->vt.video_surface_query_get_put_bits_y_cb_cr_capabilities(
        device, type, fmt, ok);
}
