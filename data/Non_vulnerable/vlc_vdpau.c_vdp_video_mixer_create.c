}
VdpStatus vdp_video_mixer_create(const vdp_t *vdp, VdpDevice device,
    uint32_t featc, const VdpVideoMixerFeature *featv,
    uint32_t parmc, const VdpVideoMixerParameter *parmv,
    const void *const *parmvalv, VdpVideoMixer *mixer)
{
    return vdp->vt.video_mixer_create(device, featc, featv, parmc, parmv,
                                      parmvalv, mixer);
}
