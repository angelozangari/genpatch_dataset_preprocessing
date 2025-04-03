/*** Video surface ***/
VdpStatus vdp_video_surface_query_capabilities(const vdp_t *vdp, VdpDevice dev,
    VdpChromaType type, VdpBool *ok, uint32_t *mw, uint32_t *mh)
{
    return vdp->vt.video_surface_query_capabilities(dev, type, ok, mw, mh);
}
