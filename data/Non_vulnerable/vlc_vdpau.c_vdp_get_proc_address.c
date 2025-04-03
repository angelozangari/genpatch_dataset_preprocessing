}
VdpStatus vdp_get_proc_address(const vdp_t *vdp, VdpDevice device,
    VdpFuncId func_id, void **func_ptr)
{
    return vdp->vt.get_proc_address(device, func_id, func_ptr);
}
