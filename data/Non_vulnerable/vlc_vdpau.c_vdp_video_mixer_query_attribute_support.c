}
VdpStatus vdp_video_mixer_query_attribute_support(const vdp_t *vdp,
    VdpDevice device, VdpVideoMixerAttribute attribute, VdpBool *ok)
{
    return vdp->vt.video_mixer_query_attribute_support(device, attribute, ok);
}
