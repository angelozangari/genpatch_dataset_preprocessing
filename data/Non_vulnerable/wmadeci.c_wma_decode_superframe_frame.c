*/
int wma_decode_superframe_frame(WMADecodeContext* s,
                                int32_t* samples, /*output*/
                                uint8_t *buf,  /*input*/
                                int buf_size)
{
    int pos, len;
    uint8_t *q;
    int done = 0;
    if ((s->use_bit_reservoir) && (s->current_frame == 0))
    {
        if (s->last_superframe_len > 0)
        {
            /* add s->bit_offset bits to last frame */
            if ((s->last_superframe_len + ((s->bit_offset + 7) >> 3)) >
                    MAX_CODED_SUPERFRAME_SIZE)
            {
#ifdef WMA_DEBUG
                printf("superframe size too large error\n");
#endif
                goto fail;
            }
            q = s->last_superframe + s->last_superframe_len;
            len = s->bit_offset;
            while (len > 0)
            {
                *q++ = (get_bits)(&s->gb, 8);
                len -= 8;
            }
            if (len > 0)
            {
                *q++ = (get_bits)(&s->gb, len) << (8 - len);
            }
            /* XXX: s->bit_offset bits into last frame */
            init_get_bits(&s->gb, s->last_superframe, MAX_CODED_SUPERFRAME_SIZE*8);
            /* skip unused bits */
            if (s->last_bitoffset > 0)
                skip_bits(&s->gb, s->last_bitoffset);
            /* this frame is stored in the last superframe and in the
               current one */
            if (wma_decode_frame(s, samples) < 0)
            {
                goto fail;
            }
            done = 1;
        }
        /* read each frame starting from s->bit_offset */
        pos = s->bit_offset + 4 + 4 + s->byte_offset_bits + 3;
        init_get_bits(&s->gb, buf + (pos >> 3), (MAX_CODED_SUPERFRAME_SIZE - (pos >> 3))*8);
        len = pos & 7;
        if (len > 0)
            skip_bits(&s->gb, len);
        s->reset_block_lengths = 1;
    }
    /* If we haven't decoded a frame yet, do it now */
    if (!done)
        {
            if (wma_decode_frame(s, samples) < 0)
            {
                goto fail;
            }
        }
    s->current_frame++;
    if ((s->use_bit_reservoir) && (s->current_frame == s->nb_frames))
    {
        /* we copy the end of the frame in the last frame buffer */
        pos = get_bits_count(&s->gb) + ((s->bit_offset + 4 + 4 + s->byte_offset_bits + 3) & ~7);
        s->last_bitoffset = pos & 7;
        pos >>= 3;
        len = buf_size - pos;
        if (len > MAX_CODED_SUPERFRAME_SIZE || len < 0)
        {
#ifdef WMA_DEBUG
            printf("superframe size too large error after decodeing\n");
#endif
            goto fail;
        }
        s->last_superframe_len = len;
        memcpy(s->last_superframe, buf + pos, len);
    }
    return s->frame_len;
fail:
    /* when error, we reset the bit reservoir */
    s->last_superframe_len = 0;
    return -1;
}
