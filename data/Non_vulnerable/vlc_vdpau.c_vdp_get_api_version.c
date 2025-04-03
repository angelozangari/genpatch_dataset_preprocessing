}
VdpStatus vdp_get_api_version(const vdp_t *vdp, uint32_t *ver)
{
    return vdp->vt.get_api_version(ver);
}
