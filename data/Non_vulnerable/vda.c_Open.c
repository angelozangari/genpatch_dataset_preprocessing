}
static int Open( vlc_va_t *external, AVCodecContext *avctx,
                 const es_format_t *fmt )
{
    msg_Dbg( external, "VDA decoder Open");
    vlc_va_vda_t *p_va = calloc( 1, sizeof(*p_va) );
    if (!p_va) {
        av_vda_default_free(avctx);
        return VLC_EGENERIC;
    }
    p_va->p_log = VLC_OBJECT(external);
    p_va->avctx = avctx;
    external->sys = p_va;
    external->description = (char *)"VDA";
    external->pix_fmt = AV_PIX_FMT_VDA;
    external->setup = Setup;
    external->get = Get;
    external->release = Release;
    external->extract = Extract;
    msg_Dbg( external, "VDA decoder Open success!");
    (void) fmt;
    return VLC_SUCCESS;
}
