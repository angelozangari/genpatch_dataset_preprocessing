 ****************************************************************************/
static picture_t *Decode(decoder_t *dec, block_t **pp_block)
{
    struct vpx_codec_ctx *ctx = &dec->p_sys->ctx;
    if( !pp_block || !*pp_block )
        return NULL;
    block_t *block = *pp_block;
    if (block->i_flags & (BLOCK_FLAG_DISCONTINUITY|BLOCK_FLAG_CORRUPTED))
        return NULL;
    /* Associate packet PTS with decoded frame */
    mtime_t *pkt_pts = malloc(sizeof(*pkt_pts));
    if (!pkt_pts) {
        block_Release(block);
        *pp_block = NULL;
        return NULL;
    }
    *pkt_pts = block->i_pts;
    vpx_codec_err_t err;
    err = vpx_codec_decode(ctx, block->p_buffer, block->i_buffer, pkt_pts, 0);
    block_Release(block);
    *pp_block = NULL;
    if (err != VPX_CODEC_OK) {
        free(pkt_pts);
        const char *error  = vpx_codec_error(ctx);
        const char *detail = vpx_codec_error_detail(ctx);
        if (!detail)
            detail = "no specific information";
        msg_Err(dec, "Failed to decode frame: %s (%s)", error, detail);
        return NULL;
    }
    const void *iter = NULL;
    struct vpx_image *img = vpx_codec_get_frame(ctx, &iter);
    if (!img) {
        free(pkt_pts);
        return NULL;
    }
    /* fetches back the PTS */
    pkt_pts = img->user_priv;
    mtime_t pts = *pkt_pts;
    free(pkt_pts);
    if (img->fmt != VPX_IMG_FMT_I420) {
        msg_Err(dec, "Unsupported output colorspace %d", img->fmt);
        return NULL;
    }
    video_format_t *v = &dec->fmt_out.video;
    if (img->d_w != v->i_visible_width || img->d_h != v->i_visible_height) {
        v->i_visible_width = img->d_w;
        v->i_visible_height = img->d_h;
    }
    picture_t *pic = decoder_NewPicture(dec);
    if (!pic)
        return NULL;
    for (int plane = 0; plane < pic->i_planes; plane++ ) {
        uint8_t *src = img->planes[plane];
        uint8_t *dst = pic->p[plane].p_pixels;
        int src_stride = img->stride[plane];
        int dst_stride = pic->p[plane].i_pitch;
        int size = __MIN( src_stride, dst_stride );
        for( int line = 0; line < pic->p[plane].i_visible_lines; line++ ) {
            memcpy( dst, src, size );
            src += src_stride;
            dst += dst_stride;
        }
    }
    pic->b_progressive = true; /* codec does not support interlacing */
    pic->date = pts;
    return pic;
}
