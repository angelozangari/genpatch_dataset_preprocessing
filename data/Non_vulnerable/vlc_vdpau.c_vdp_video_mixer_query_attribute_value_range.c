}
VdpStatus vdp_video_mixer_query_attribute_value_range(const vdp_t *vdp,
    VdpDevice device, VdpVideoMixerAttribute attribute, void *min, void *max)
{
    return vdp->vt.video_mixer_query_attribute_value_range(device, attribute,
        min, max);
}
