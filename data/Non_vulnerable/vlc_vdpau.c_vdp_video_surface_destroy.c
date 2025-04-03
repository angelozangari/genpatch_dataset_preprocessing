}
VdpStatus vdp_video_surface_destroy(const vdp_t *vdp, VdpVideoSurface surface)
{
    return vdp->vt.video_surface_destroy(surface);
}
