/*** Preemption ***/
VdpStatus vdp_preemption_callback_register(const vdp_t *vdp, VdpDevice device,
    VdpPreemptionCallback cb, void *ctx)
{
    return vdp->vt.preemption_callback_register(device, cb, ctx);
}
