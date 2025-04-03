}
static void video_unlink_picture_decoder( decoder_t *p_dec, picture_t *p_pic )
{
    VLC_UNUSED(p_dec);
    picture_Release( p_pic );
}
