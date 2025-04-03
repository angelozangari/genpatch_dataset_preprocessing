}
VdpStatus vdp_presentation_queue_display(const vdp_t *vdp,
    VdpPresentationQueue queue, VdpOutputSurface surface, uint32_t clip_width,
    uint32_t clip_height, VdpTime pts)
{
    return vdp->vt.presentation_queue_display(queue, surface, clip_width,
                                              clip_height, pts);
}
