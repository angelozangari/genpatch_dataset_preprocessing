};
static void video_del_buffer_decoder( decoder_t *p_decoder, picture_t *p_pic )
{
    VLC_UNUSED(p_decoder);
    picture_Release( p_pic );
}
