};
const char *vdp_get_error_string(const vdp_t *vdp, VdpStatus status)
{
    return vdp->vt.get_error_string(status);
}
