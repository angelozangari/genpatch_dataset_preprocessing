/* decode a frame of frame_len samples */
static int wma_decode_frame(WMADecodeContext *s, int32_t *samples)
{
    int ret, i, n, ch, incr;
    int32_t *ptr;
    int32_t *iptr;
    /* read each block */
    s->block_num = 0;
    s->block_pos = 0;
    for(;;)
    {
        ret = wma_decode_block(s);
        if (ret < 0)
        {
#ifdef WMA_DEBUG
            printf("wma_decode_block failed with code %d\n", ret);
#endif
            return -1;
        }
        if (ret)
        {
            break;
        }
    }
    /* return frame with full 30-bit precision */
    n = s->frame_len;
    incr = s->nb_channels;
    for(ch = 0; ch < s->nb_channels; ++ch)
    {
        ptr = samples + ch;
        iptr = s->frame_out[ch];
        for (i=0;i<n;++i)
        {
            *ptr = (*iptr++);
            ptr += incr;
        }
        /* prepare for next block */
        memmove(&s->frame_out[ch][0], &s->frame_out[ch][s->frame_len],
                s->frame_len * sizeof(int32_t));
    }
    return 0;
}
