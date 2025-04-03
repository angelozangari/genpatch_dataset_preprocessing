}
VdpStatus vdp_video_surface_get_parameters(const vdp_t *vdp,
    VdpVideoSurface surface, VdpChromaType *type, uint32_t *w, uint32_t *h)
{
    return vdp->vt.video_surface_get_parameters(surface, type, w, h);
}
