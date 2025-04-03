}
static filter_t *SpuRenderCreateAndLoadScale(vlc_object_t *object,
                                             vlc_fourcc_t src_chroma,
                                             vlc_fourcc_t dst_chroma,
                                             bool require_resize)
{
    filter_t *scale = vlc_custom_create(object, sizeof(*scale), "scale");
    if (!scale)
        return NULL;
    es_format_Init(&scale->fmt_in, VIDEO_ES, 0);
    scale->fmt_in.video.i_chroma = src_chroma;
    scale->fmt_in.video.i_width =
    scale->fmt_in.video.i_visible_width =
    scale->fmt_in.video.i_height =
    scale->fmt_in.video.i_visible_height = 32;
    es_format_Init(&scale->fmt_out, VIDEO_ES, 0);
    scale->fmt_out.video.i_chroma = dst_chroma;
    scale->fmt_out.video.i_width =
    scale->fmt_out.video.i_visible_width =
    scale->fmt_out.video.i_height =
    scale->fmt_out.video.i_visible_height = require_resize ? 16 : 32;
    scale->pf_video_buffer_new = spu_new_video_buffer;
    scale->pf_video_buffer_del = spu_del_video_buffer;
    scale->p_module = module_need(scale, "video filter2", NULL, false);
    return scale;
}
