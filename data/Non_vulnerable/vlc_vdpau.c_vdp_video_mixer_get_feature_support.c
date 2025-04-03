}
VdpStatus vdp_video_mixer_get_feature_support(const vdp_t *vdp,
    VdpVideoMixer mixer, uint32_t count, const VdpVideoMixerFeature *ids,
    VdpBool *values)
{
    return vdp->vt.video_mixer_get_feature_support(mixer, count, ids, values);
}
