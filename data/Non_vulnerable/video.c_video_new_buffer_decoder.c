}
static picture_t *video_new_buffer_decoder( decoder_t *p_dec )
{
    p_dec->fmt_out.video.i_chroma = p_dec->fmt_out.i_codec;
    return picture_NewFromFormat( &p_dec->fmt_out.video );
}
