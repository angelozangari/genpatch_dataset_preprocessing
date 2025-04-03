}
VdpStatus vdp_presentation_queue_destroy(const vdp_t *vdp,
    VdpPresentationQueue queue)
{
    return vdp->vt.presentation_queue_destroy(queue);
}
