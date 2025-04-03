}
VdpStatus vdp_video_mixer_set_feature_enables(const vdp_t *vdp,
    VdpVideoMixer mixer, uint32_t count, const VdpVideoMixerFeature *ids,
    const VdpBool *values)
{
    return vdp->vt.video_mixer_set_feature_enables(mixer, count, ids, values);
}
