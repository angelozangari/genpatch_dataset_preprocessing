}
VdpStatus vdp_presentation_queue_query_surface_status(const vdp_t *vdp,
    VdpPresentationQueue queue, VdpOutputSurface surface,
    VdpPresentationQueueStatus *status, VdpTime *pts)
{
    return vdp->vt.presentation_queue_query_surface_status(queue, surface,
                                                           status, pts);
}
