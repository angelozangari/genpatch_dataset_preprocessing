static picture_t *Decode(decoder_t *, block_t **);
static int Open(vlc_object_t *obj)
{
    decoder_t *dec = (decoder_t *)obj;
    if (dec->fmt_in.i_codec != VLC_CODEC_XWD)
        return VLC_EGENERIC;
    dec->pf_decode_video = Decode;
    es_format_Copy(&dec->fmt_out, &dec->fmt_in);
    dec->fmt_out.i_codec = VLC_CODEC_RGB32;
    dec->fmt_out.i_cat = VIDEO_ES;
    return VLC_SUCCESS;
}
