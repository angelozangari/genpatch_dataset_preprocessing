 *****************************************************************************/
static int Open(vlc_object_t *p_this)
{
    decoder_t *dec = (decoder_t *)p_this;
    const struct vpx_codec_iface *iface;
    int vp_version;
    switch (dec->fmt_in.i_codec)
    {
#ifdef ENABLE_VP8_DECODER
    case VLC_CODEC_VP8:
        iface = &vpx_codec_vp8_dx_algo;
        vp_version = 8;
        break;
#endif
#ifdef ENABLE_VP9_DECODER
    case VLC_CODEC_VP9:
        iface = &vpx_codec_vp9_dx_algo;
        vp_version = 9;
        break;
#endif
    default:
        return VLC_EGENERIC;
    }
    decoder_sys_t *sys = malloc(sizeof(*sys));
    if (!sys)
        return VLC_ENOMEM;
    dec->p_sys = sys;
    struct vpx_codec_dec_cfg deccfg = {
        .threads = __MIN(vlc_GetCPUCount(), 16)
    };
    msg_Dbg(p_this, "VP%d: using libvpx version %s (build options %s)", 
        vp_version, vpx_codec_version_str(), vpx_codec_build_config());
    if (vpx_codec_dec_init(&sys->ctx, iface, &deccfg, 0) != VPX_CODEC_OK) {
        const char *error = vpx_codec_error(&sys->ctx);
        msg_Err(p_this, "Failed to initialize decoder: %s\n", error);
        free(sys);
        return VLC_EGENERIC;;
    }
    dec->pf_decode_video = Decode;
    dec->fmt_out.i_cat = VIDEO_ES;
    dec->fmt_out.video.i_width = dec->fmt_in.video.i_width;
    dec->fmt_out.video.i_height = dec->fmt_in.video.i_height;
    dec->fmt_out.i_codec = VLC_CODEC_I420;
    dec->b_need_packetized = true;
    return VLC_SUCCESS;
}
