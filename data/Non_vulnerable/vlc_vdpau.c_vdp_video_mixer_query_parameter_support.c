}
VdpStatus vdp_video_mixer_query_parameter_support(const vdp_t *vdp,
    VdpDevice device, VdpVideoMixerParameter parameter, VdpBool *ok)
{
    return vdp->vt.video_mixer_query_parameter_support(device, parameter, ok);
}
