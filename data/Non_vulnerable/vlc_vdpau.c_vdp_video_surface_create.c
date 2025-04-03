}
VdpStatus vdp_video_surface_create(const vdp_t *vdp, VdpDevice device,
    VdpChromaType chroma, uint32_t w, uint32_t h, VdpVideoSurface *surface)
{
    return vdp->vt.video_surface_create(device, chroma, w, h, surface);
}
