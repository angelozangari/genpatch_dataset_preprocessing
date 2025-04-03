}
VdpStatus vdp_presentation_queue_block_until_surface_idle(const vdp_t *vdp,
    VdpPresentationQueue queue, VdpOutputSurface surface, VdpTime *pts)
{
    return vdp->vt.presentation_queue_block_until_surface_idle(queue, surface,
                                                               pts);
}
