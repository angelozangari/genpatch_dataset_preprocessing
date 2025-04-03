}
static void spu_del_video_buffer(filter_t *filter, picture_t *picture)
{
    VLC_UNUSED(filter);
    picture_Release(picture);
}
