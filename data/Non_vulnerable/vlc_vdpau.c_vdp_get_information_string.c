}
VdpStatus vdp_get_information_string(const vdp_t *vdp, const char **str)
{
    return vdp->vt.get_information_string(str);
}
