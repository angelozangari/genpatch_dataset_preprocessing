******************************************************************************/
static void ParseHeader( decoder_t *p_dec, block_t *p_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    uint8_t *p = p_block->p_buffer;
    uint8_t i_options, i_options2, i_cmd, i_cmd_arg;
    int i;
    p_sys->i_spu_size = GETINT16(p);
    i_options  = *p++;
    i_options2 = *p++;
    if( i_options & 0x08 ) { p_sys->i_duration = GETINT32(p); }
    else p_sys->i_duration = 0; /* Ephemer subtitle */
    p_sys->i_duration *= 100 / 9;
    p_sys->i_x_start = GETINT16(p);
    p_sys->i_y_start = GETINT16(p);
    p_sys->i_width   = GETINT16(p);
    p_sys->i_height  = GETINT16(p);
    for( i = 0; i < 4; i++ )
    {
        p_sys->p_palette[i][0] = *p++; /* Y */
        p_sys->p_palette[i][2] = *p++; /* Cr / V */
        p_sys->p_palette[i][1] = *p++; /* Cb / U */
        p_sys->p_palette[i][3] = *p++; /* T */
    }
    i_cmd = *p++;
    /* We do not really know this, FIXME */
    if( i_cmd ) {i_cmd_arg = GETINT32(p);}
    /* Actually, this is measured against a different origin, so we have to
     * adjust it */
    p_sys->second_field_offset = GETINT16(p);
    p_sys->i_image_offset  = p - p_block->p_buffer;
    p_sys->i_image_length  = p_sys->i_spu_size - p_sys->i_image_offset;
    p_sys->metadata_length = p_sys->i_image_offset;
#ifndef NDEBUG
    msg_Dbg( p_dec, "x-start: %d, y-start: %d, width: %d, height %d, "
             "spu size: %zu, duration: %"PRIu64" (d:%zu p:%"PRIu16")",
             p_sys->i_x_start, p_sys->i_y_start,
             p_sys->i_width, p_sys->i_height,
             p_sys->i_spu_size, p_sys->i_duration,
             p_sys->i_image_length, p_sys->i_image_offset);
    for( i = 0; i < 4; i++ )
    {
        msg_Dbg( p_dec, "palette[%d]= T: %2x, Y: %2x, u: %2x, v: %2x", i,
                 p_sys->p_palette[i][3], p_sys->p_palette[i][0],
                 p_sys->p_palette[i][1], p_sys->p_palette[i][2] );
    }
#endif
}
