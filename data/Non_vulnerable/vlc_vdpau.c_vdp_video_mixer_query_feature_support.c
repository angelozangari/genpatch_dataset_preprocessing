/*** Video mixer ***/
VdpStatus vdp_video_mixer_query_feature_support(const vdp_t *vdp,
    VdpDevice device, VdpVideoMixerFeature feature, VdpBool *ok)
{
    return vdp->vt.video_mixer_query_feature_support(device, feature, ok);
}
