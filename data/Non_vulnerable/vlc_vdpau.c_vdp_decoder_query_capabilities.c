/*** Decoder ***/
VdpStatus vdp_decoder_query_capabilities(const vdp_t *vdp, VdpDevice device,
    VdpDecoderProfile profile, VdpBool *ok, uint32_t *l, uint32_t *m,
    uint32_t *w, uint32_t *h)
{
    return vdp->vt.decoder_query_capabilities(device, profile, ok, l, m, w, h);
}
