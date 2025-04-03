}
static void VideoFormatCopyCropAr(video_format_t *dst,
                                  const video_format_t *src)
{
    video_format_CopyCrop(dst, src);
    dst->i_sar_num = src->i_sar_num;
    dst->i_sar_den = src->i_sar_den;
}
