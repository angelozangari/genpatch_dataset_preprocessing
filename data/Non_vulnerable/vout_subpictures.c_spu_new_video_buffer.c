}
static picture_t *spu_new_video_buffer(filter_t *filter)
{
    const video_format_t *fmt = &filter->fmt_out.video;
    VLC_UNUSED(filter);
    return picture_NewFromFormat(fmt);
}
