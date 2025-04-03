}
VdpStatus vdp_presentation_queue_set_background_color(const vdp_t *vdp,
    VdpPresentationQueue queue, const VdpColor *color)
{
    VdpColor bak = *color;
    return vdp->vt.presentation_queue_set_background_color(queue, &bak);
}
