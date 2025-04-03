};
static block_t *Decode(decoder_t *dec, block_t **block_ptr)
{
    decoder_sys_t *sys  = dec->p_sys;
    if (!block_ptr || !*block_ptr)
        return NULL;
    block_t *block = *block_ptr;
    if (block->i_flags & (BLOCK_FLAG_DISCONTINUITY | BLOCK_FLAG_CORRUPTED)) {
        if (block->i_flags & BLOCK_FLAG_CORRUPTED) {
        }
        date_Set(&sys->end_date, 0);
        block_Release(block);
        return NULL;
    }
    if (block->i_pts > VLC_TS_INVALID &&
        block->i_pts != date_Get(&sys->end_date))
        date_Set(&sys->end_date, block->i_pts);
    block->i_pts = VLC_TS_INVALID;
    if (!date_Get(&sys->end_date)) {
        /* We've just started the stream, wait for the first PTS. */
        block_Release(block);
        return NULL;
    }
    const unsigned int block_size = sys->is_pal ? 8640 : 7200;
    if (block->i_buffer >= block_size) {
        uint8_t *src = block->p_buffer;
        block->i_buffer -= block_size;
        block->p_buffer += block_size;
        int sample_count = dv_get_audio_sample_count(&src[244], sys->is_pal);
        block_t *output = decoder_NewAudioBuffer(dec, sample_count);
        if (!output)
            return NULL;
        output->i_pts    = date_Get(&sys->end_date);
        output->i_length = date_Increment(&sys->end_date, sample_count) - output->i_pts;
        int16_t *dst = (int16_t*)output->p_buffer;
        for (int i = 0; i < sample_count; i++) {
          const uint8_t *v = &src[sys->shuffle[i]];
          if (sys->is_12bit) {
              *dst++ = dv_audio_12to16((v[0] << 4) | ((v[2] >> 4) & 0x0f));
              *dst++ = dv_audio_12to16((v[1] << 4) | ((v[2] >> 0) & 0x0f));
          } else {
              *dst++ = GetWBE(&v[0]);
              *dst++ = GetWBE(&v[sys->is_pal ? 4320 : 3600]);
          }
        }
        return output;
    }
    block_Release(block);
    return NULL;
}
