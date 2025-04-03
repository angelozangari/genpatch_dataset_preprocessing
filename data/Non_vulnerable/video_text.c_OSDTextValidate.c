};
static int OSDTextValidate(subpicture_t *subpic,
                           bool has_src_changed, const video_format_t *fmt_src,
                           bool has_dst_changed, const video_format_t *fmt_dst,
                           mtime_t ts)
{
    VLC_UNUSED(subpic); VLC_UNUSED(ts);
    VLC_UNUSED(fmt_src); VLC_UNUSED(has_src_changed);
    VLC_UNUSED(fmt_dst);
    if( !has_dst_changed )
        return VLC_SUCCESS;
    return VLC_EGENERIC;
}
