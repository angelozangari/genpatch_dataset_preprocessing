}
VdpStatus vdp_decoder_render(const vdp_t *vdp, VdpDecoder decoder,
    VdpVideoSurface target, const VdpPictureInfo *info,
    uint32_t bufv, const VdpBitstreamBuffer *bufc)
{
    return vdp->vt.decoder_render(decoder, target, info, bufv, bufc);
}
