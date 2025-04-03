}
VdpStatus vdp_video_mixer_query_parameter_value_range(const vdp_t *vdp,
    VdpDevice device, VdpVideoMixerParameter parameter, void *min, void *max)
{
    return vdp->vt.video_mixer_query_parameter_value_range(device, parameter,
        min, max);
}
