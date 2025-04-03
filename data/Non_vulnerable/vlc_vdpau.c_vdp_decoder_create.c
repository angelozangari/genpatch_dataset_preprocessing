}
VdpStatus vdp_decoder_create(const vdp_t *vdp, VdpDevice device,
    VdpDecoderProfile profile, uint32_t w, uint32_t h, uint32_t refs,
    VdpDecoder *decoder)
{
    return vdp->vt.decoder_create(device, profile, w, h, refs, decoder);
}
