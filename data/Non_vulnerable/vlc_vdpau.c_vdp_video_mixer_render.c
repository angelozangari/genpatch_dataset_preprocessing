}
VdpStatus vdp_video_mixer_render(const vdp_t *vdp, VdpVideoMixer mixer,
    VdpOutputSurface bgsurface, const VdpRect *bgrect,
    VdpVideoMixerPictureStructure pic_struct, uint32_t prev_count,
    const VdpVideoSurface *prev, VdpVideoSurface cur, uint32_t next_count,
    const VdpVideoSurface *next, const VdpRect *src_rect,
    VdpOutputSurface dst, const VdpRect *dst_rect, const VdpRect *dst_v_rect,
    uint32_t layerc, const VdpLayer *layerv)
{
    return vdp->vt.video_mixer_render(mixer, bgsurface, bgrect, pic_struct,
        prev_count, prev, cur, next_count, next, src_rect, dst, dst_rect,
        dst_v_rect, layerc, layerv);
}
