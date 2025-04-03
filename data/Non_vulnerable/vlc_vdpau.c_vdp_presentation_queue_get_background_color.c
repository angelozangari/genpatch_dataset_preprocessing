}
VdpStatus vdp_presentation_queue_get_background_color(const vdp_t *vdp,
    VdpPresentationQueue queue, VdpColor *color)
{
    return vdp->vt.presentation_queue_get_background_color(queue, color);
}
