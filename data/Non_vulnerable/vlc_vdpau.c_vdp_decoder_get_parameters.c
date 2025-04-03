}
VdpStatus vdp_decoder_get_parameters(const vdp_t *vdp, VdpDecoder decoder,
    VdpDecoderProfile *profile, uint32_t *w, uint32_t *h)
{
    return vdp->vt.decoder_get_parameters(decoder, profile, w, h);
}
