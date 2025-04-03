}
static block_t *Add_ADTS( block_t *p_data, es_format_t *p_fmt )
{
#define ADTS_HEADER_SIZE 7 /* CRC needs 2 more bytes */
    uint8_t *p_extra = p_fmt->p_extra;
    if( !p_data || p_fmt->i_extra < 2 || !p_extra )
        return p_data; /* no data to construct the headers */
    size_t frame_length = p_data->i_buffer + ADTS_HEADER_SIZE;
    int i_index = ( (p_extra[0] << 1) | (p_extra[1] >> 7) ) & 0x0f;
    int i_profile = (p_extra[0] >> 3) - 1; /* i_profile < 4 */
    if( i_index == 0x0f && p_fmt->i_extra < 5 )
        return p_data; /* not enough data */
    int i_channels = (p_extra[i_index == 0x0f ? 4 : 1] >> 3) & 0x0f;
    /* keep a copy in case block_Realloc() fails */
    block_t *p_bak_block = block_Duplicate( p_data );
    if( !p_bak_block ) /* OOM, block_Realloc() is likely to lose our block */
        return p_data; /* the frame isn't correct but that's the best we have */
    block_t *p_new_block = block_Realloc( p_data, ADTS_HEADER_SIZE,
                                            p_data->i_buffer );
    if( !p_new_block )
        return p_bak_block; /* OOM, send the (incorrect) original frame */
    block_Release( p_bak_block ); /* we don't need the copy anymore */
    uint8_t *p_buffer = p_new_block->p_buffer;
    /* fixed header */
    p_buffer[0] = 0xff;
    p_buffer[1] = 0xf1; /* 0xf0 | 0x00 | 0x00 | 0x01 */
    p_buffer[2] = (i_profile << 6) | ((i_index & 0x0f) << 2) | ((i_channels >> 2) & 0x01) ;
    p_buffer[3] = (i_channels << 6) | ((frame_length >> 11) & 0x03);
    /* variable header (starts at last 2 bits of 4th byte) */
    int i_fullness = 0x7ff; /* 0x7ff means VBR */
    /* XXX: We should check if it's CBR or VBR, but no known implementation
     * do that, and it's a pain to calculate this field */
    p_buffer[4] = frame_length >> 3;
    p_buffer[5] = ((frame_length & 0x07) << 5) | ((i_fullness >> 6) & 0x1f);
    p_buffer[6] = ((i_fullness & 0x3f) << 2) /* | 0xfc */;
    return p_new_block;
}
