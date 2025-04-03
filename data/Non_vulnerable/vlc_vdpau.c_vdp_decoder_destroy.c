}
VdpStatus vdp_decoder_destroy(const vdp_t *vdp, VdpDecoder decoder)
{
    return vdp->vt.decoder_destroy(decoder);
}
