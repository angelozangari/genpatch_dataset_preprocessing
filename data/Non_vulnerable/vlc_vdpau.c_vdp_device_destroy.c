/*** Device ***/
VdpStatus vdp_device_destroy(const vdp_t *vdp, VdpDevice device)
{
    return vdp->vt.device_destroy(device);
}
