}
VdpStatus vdp_video_mixer_destroy(const vdp_t *vdp, VdpVideoMixer mixer)
{
    return vdp->vt.video_mixer_destroy(mixer);
}
