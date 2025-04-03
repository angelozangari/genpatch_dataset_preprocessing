}
VdpStatus vdp_presentation_queue_create(const vdp_t *vdp, VdpDevice device,
    VdpPresentationQueueTarget target, VdpPresentationQueue *queue)
{
    return vdp->vt.presentation_queue_create(device, target, queue);
}
