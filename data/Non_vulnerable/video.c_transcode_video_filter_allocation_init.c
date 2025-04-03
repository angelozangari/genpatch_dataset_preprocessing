}
static int transcode_video_filter_allocation_init( filter_t *p_filter,
                                                   void *p_data )
{
    VLC_UNUSED(p_data);
    p_filter->pf_video_buffer_new = transcode_video_filter_buffer_new;
    p_filter->pf_video_buffer_del = transcode_video_filter_buffer_del;
    return VLC_SUCCESS;
}
