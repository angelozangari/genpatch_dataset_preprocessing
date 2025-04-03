/* */
static int VoutValidateFormat(video_format_t *dst,
                              const video_format_t *src)
{
    if (src->i_width <= 0  || src->i_width  > 8192 ||
        src->i_height <= 0 || src->i_height > 8192)
        return VLC_EGENERIC;
    if (src->i_sar_num <= 0 || src->i_sar_den <= 0)
        return VLC_EGENERIC;
    /* */
    video_format_Copy(dst, src);
    dst->i_chroma = vlc_fourcc_GetCodec(VIDEO_ES, src->i_chroma);
    vlc_ureduce( &dst->i_sar_num, &dst->i_sar_den,
                 src->i_sar_num,  src->i_sar_den, 50000 );
    if (dst->i_sar_num <= 0 || dst->i_sar_den <= 0) {
        dst->i_sar_num = 1;
        dst->i_sar_den = 1;
    }
    video_format_FixRgb(dst);
    return VLC_SUCCESS;
}
