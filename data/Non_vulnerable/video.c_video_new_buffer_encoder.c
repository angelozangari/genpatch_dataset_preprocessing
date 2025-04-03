}
static picture_t *video_new_buffer_encoder( encoder_t *p_enc )
{
    p_enc->fmt_in.video.i_chroma = p_enc->fmt_in.i_codec;
    return picture_NewFromFormat( &p_enc->fmt_in.video );
}
