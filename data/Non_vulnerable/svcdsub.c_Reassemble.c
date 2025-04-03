#define SPU_HEADER_LEN 5
static block_t *Reassemble( decoder_t *p_dec, block_t *p_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    uint8_t *p_buffer;
    uint16_t i_expected_image;
    uint8_t  i_packet, i_expected_packet;
    if( p_block->i_flags & (BLOCK_FLAG_DISCONTINUITY|BLOCK_FLAG_CORRUPTED) )
    {
        block_Release( p_block );
        return NULL;
    }
    if( p_block->i_buffer < SPU_HEADER_LEN )
    {
        msg_Dbg( p_dec, "invalid packet header (size %zu < %u)" ,
                 p_block->i_buffer, SPU_HEADER_LEN );
        block_Release( p_block );
        return NULL;
    }
    p_buffer = p_block->p_buffer;
    if( p_sys->i_state == SUBTITLE_BLOCK_EMPTY )
    {
        i_expected_image  = p_sys->i_image + 1;
        i_expected_packet = 0;
    }
    else
    {
        i_expected_image  = p_sys->i_image;
        i_expected_packet = p_sys->i_packet + 1;
    }
    /* The dummy ES that the menu selection uses has an 0x70 at
       the head which we need to strip off. */
    p_buffer += 2;
    if( *p_buffer & 0x80 )
    {
        p_sys->i_state = SUBTITLE_BLOCK_COMPLETE;
        i_packet       = *p_buffer++ & 0x7F;
    }
    else
    {
        p_sys->i_state = SUBTITLE_BLOCK_PARTIAL;
        i_packet       = *p_buffer++;
    }
    p_sys->i_image = GETINT16(p_buffer);
    if( p_sys->i_image != i_expected_image )
    {
        msg_Warn( p_dec, "expected subtitle image %u but found %u",
                  i_expected_image, p_sys->i_image );
    }
    if( i_packet != i_expected_packet )
    {
        msg_Warn( p_dec, "expected subtitle image packet %u but found %u",
                  i_expected_packet, i_packet );
    }
    p_block->p_buffer += SPU_HEADER_LEN;
    p_block->i_buffer -= SPU_HEADER_LEN;
    p_sys->i_packet = i_packet;
    /* First packet in the subtitle block */
    if( !p_sys->i_packet ) ParseHeader( p_dec, p_block );
    block_ChainAppend( &p_sys->p_spu, p_block );
    if( p_sys->i_state == SUBTITLE_BLOCK_COMPLETE )
    {
        block_t *p_spu = block_ChainGather( p_sys->p_spu );
        if( p_spu->i_buffer != p_sys->i_spu_size )
        {
            msg_Warn( p_dec, "subtitle packets size=%zu should be %zu",
                      p_spu->i_buffer, p_sys->i_spu_size );
        }
        dbg_print( "subtitle packet complete, size=%zu", p_spu->i_buffer );
        p_sys->i_state = SUBTITLE_BLOCK_EMPTY;
        p_sys->p_spu = 0;
        return p_spu;
    }
    return NULL;
}
