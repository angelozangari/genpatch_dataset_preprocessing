}
static void transcode_video_filter_buffer_del( filter_t *p_filter, picture_t *p_pic )
{
    VLC_UNUSED(p_filter);
    picture_Release( p_pic );
}
