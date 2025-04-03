};
static block_t *Encode(encoder_t *p_enc, picture_t *p_pict)
{
    encoder_sys_t *p_sys = p_enc->p_sys;
    x265_picture pic;
    x265_picture_init(&p_sys->param, &pic);
    if (likely(p_pict)) {
        pic.pts = p_pict->date;
        if (unlikely(p_sys->initial_date == 0)) {
            p_sys->initial_date = p_pict->date;
#ifndef NDEBUG
            p_sys->start = mdate();
#endif
        }
        for (int i = 0; i < p_pict->i_planes; i++) {
            pic.planes[i] = p_pict->p[i].p_pixels;
            pic.stride[i] = p_pict->p[i].i_pitch;
        }
    }
    x265_nal *nal;
    uint32_t i_nal = 0;
    x265_encoder_encode(p_sys->h, &nal, &i_nal,
            likely(p_pict) ? &pic : NULL, &pic);
    if (!i_nal)
        return NULL;
    int i_out = 0;
    for (uint32_t i = 0; i < i_nal; i++)
        i_out += nal[i].sizeBytes;
    block_t *p_block = block_Alloc(i_out);
    if (!p_block)
        return NULL;
    /* all payloads are sequentially laid out in memory */
    memcpy(p_block->p_buffer, nal[0].payload, i_out);
    /* This isn't really valid for streams with B-frames */
    p_block->i_length = CLOCK_FREQ *
        p_enc->fmt_in.video.i_frame_rate_base /
            p_enc->fmt_in.video.i_frame_rate;
    p_block->i_pts = p_sys->initial_date + pic.poc * p_block->i_length;
    p_block->i_dts = p_sys->initial_date + p_sys->dts++ * p_block->i_length;
    switch (pic.sliceType)
    {
    case X265_TYPE_I:
    case X265_TYPE_IDR:
        p_block->i_flags |= BLOCK_FLAG_TYPE_I;
        break;
    case X265_TYPE_P:
        p_block->i_flags |= BLOCK_FLAG_TYPE_P;
        break;
    case X265_TYPE_B:
    case X265_TYPE_BREF:
        p_block->i_flags |= BLOCK_FLAG_TYPE_B;
        break;
    }
#ifndef NDEBUG
    msg_Dbg(p_enc, "%zu bytes (frame %"PRId64", %.2ffps)", p_block->i_buffer,
        p_sys->dts, (float)p_sys->dts * CLOCK_FREQ / (mdate() - p_sys->start));
#endif
    return p_block;
}
