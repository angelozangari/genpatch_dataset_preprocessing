}
VdpStatus vdp_output_surface_render_output_surface(const vdp_t *vdp,
    VdpOutputSurface dst_surface, const VdpRect *dst_rect,
    VdpOutputSurface src_surface, const VdpRect *src_rect,
    const VdpColor *colors,
    const VdpOutputSurfaceRenderBlendState *const state, uint32_t flags)
{
    return vdp->vt.output_surface_render_output_surface(dst_surface, dst_rect,
        src_surface, src_rect, colors, state, flags);
}
