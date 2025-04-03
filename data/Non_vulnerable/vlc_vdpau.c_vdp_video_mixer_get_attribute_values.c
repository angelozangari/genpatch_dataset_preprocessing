}
VdpStatus vdp_video_mixer_get_attribute_values(const vdp_t *vdp,
    VdpVideoMixer mixer, uint32_t count, const VdpVideoMixerAttribute *ids,
    void *const *values)
{
    return vdp->vt.video_mixer_get_attribute_values(mixer, count, ids, values);
}
