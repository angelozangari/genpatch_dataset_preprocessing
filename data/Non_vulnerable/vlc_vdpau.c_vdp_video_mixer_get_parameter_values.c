}
VdpStatus vdp_video_mixer_get_parameter_values(const vdp_t *vdp,
    VdpVideoMixer mixer, uint32_t count, const VdpVideoMixerParameter *ids,
    void *const *values)
{
    return vdp->vt.video_mixer_get_parameter_values(mixer, count, ids, values);
}
