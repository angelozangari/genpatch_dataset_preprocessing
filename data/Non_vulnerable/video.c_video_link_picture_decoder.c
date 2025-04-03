}
static void video_link_picture_decoder( decoder_t *p_dec, picture_t *p_pic )
{
    VLC_UNUSED(p_dec);
    picture_Hold( p_pic );
}
