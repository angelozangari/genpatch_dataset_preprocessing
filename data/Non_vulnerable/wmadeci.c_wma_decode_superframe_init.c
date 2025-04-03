/* Initialise the superframe decoding */
int wma_decode_superframe_init(WMADecodeContext* s,
                                 uint8_t *buf,  /*input*/
                                 int buf_size)
{
    if (buf_size==0)
    {
        s->last_superframe_len = 0;
        return 0;
    }
    s->current_frame = 0;
    init_get_bits(&s->gb, buf, buf_size*8);
    if (s->use_bit_reservoir)
    {
        /* read super frame header */
        get_bits(&s->gb, 4); /* super frame index */
        s->nb_frames = get_bits(&s->gb, 4);
        if (s->last_superframe_len == 0)
            s->nb_frames --;
        else if (s->nb_frames == 0)
            s->nb_frames++;
        s->bit_offset = get_bits(&s->gb, s->byte_offset_bits + 3);
    } else {
        s->nb_frames = 1;
    }
    return 1;
}
