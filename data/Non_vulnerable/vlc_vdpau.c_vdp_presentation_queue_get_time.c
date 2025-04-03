}
VdpStatus vdp_presentation_queue_get_time(const vdp_t *vdp,
    VdpPresentationQueue queue, VdpTime *current_time)
{
    return vdp->vt.presentation_queue_get_time(queue, current_time);
}
