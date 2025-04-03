/*** Presentation queue ***/
VdpStatus vdp_presentation_queue_target_destroy(const vdp_t *vdp,
    VdpPresentationQueueTarget target)
{
    return vdp->vt.presentation_queue_target_destroy(target);
}
