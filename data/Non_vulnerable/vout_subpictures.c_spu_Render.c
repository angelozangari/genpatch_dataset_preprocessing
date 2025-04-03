}
subpicture_t *spu_Render(spu_t *spu,
                         const vlc_fourcc_t *chroma_list,
                         const video_format_t *fmt_dst,
                         const video_format_t *fmt_src,
                         mtime_t render_subtitle_date,
                         mtime_t render_osd_date,
                         bool ignore_osd)
{
    spu_private_t *sys = spu->p;
    /* Update sub-source chain */
    vlc_mutex_lock(&sys->lock);
    char *chain_update = sys->source_chain_update;
    sys->source_chain_update = NULL;
    vlc_mutex_unlock(&sys->lock);
    vlc_mutex_lock(&sys->source_chain_lock);
    if (chain_update) {
        if (*chain_update) {
            filter_chain_Reset(sys->source_chain, NULL, NULL);
            filter_chain_AppendFromString(spu->p->source_chain, chain_update);
        }
        else if (filter_chain_GetLength(spu->p->source_chain) > 0)
            filter_chain_Reset(sys->source_chain, NULL, NULL);
        free(chain_update);
    }
    /* Run subpicture sources */
    filter_chain_SubSource(sys->source_chain, render_osd_date);
    vlc_mutex_unlock(&sys->source_chain_lock);
    static const vlc_fourcc_t chroma_list_default_yuv[] = {
        VLC_CODEC_YUVA,
        VLC_CODEC_RGBA,
        VLC_CODEC_ARGB,
        VLC_CODEC_YUVP,
        0,
    };
    static const vlc_fourcc_t chroma_list_default_rgb[] = {
        VLC_CODEC_RGBA,
        VLC_CODEC_ARGB,
        VLC_CODEC_YUVA,
        VLC_CODEC_YUVP,
        0,
    };
    if (!chroma_list || *chroma_list == 0)
        chroma_list = vlc_fourcc_IsYUV(fmt_dst->i_chroma) ? chroma_list_default_yuv
                                                          : chroma_list_default_rgb;
    vlc_mutex_lock(&sys->lock);
    unsigned int subpicture_count;
    subpicture_t *subpicture_array[VOUT_MAX_SUBPICTURES];
    /* Get an array of subpictures to render */
    SpuSelectSubpictures(spu, &subpicture_count, subpicture_array,
                         render_subtitle_date, render_osd_date, ignore_osd);
    if (subpicture_count <= 0) {
        vlc_mutex_unlock(&sys->lock);
        return NULL;
    }
    /* Updates the subpictures */
    for (unsigned i = 0; i < subpicture_count; i++) {
        subpicture_t *subpic = subpicture_array[i];
        subpicture_Update(subpic,
                          fmt_src, fmt_dst,
                          subpic->b_subtitle ? render_subtitle_date : render_osd_date);
    }
    /* Now order the subpicture array
     * XXX The order is *really* important for overlap subtitles positionning */
    qsort(subpicture_array, subpicture_count, sizeof(*subpicture_array), SubpictureCmp);
    /* Render the subpictures */
    subpicture_t *render = SpuRenderSubpictures(spu,
                                                subpicture_count, subpicture_array,
                                                chroma_list,
                                                fmt_dst,
                                                fmt_src,
                                                render_subtitle_date,
                                                render_osd_date);
    vlc_mutex_unlock(&sys->lock);
    return render;
}
