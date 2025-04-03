}
VdpStatus vdp_output_surface_destroy(const vdp_t *vdp,
                                     VdpOutputSurface surface)
{
    return vdp->vt.output_surface_destroy(surface);
}
