 *****************************************************************************/
static block_t *Reassemble( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t *p_block;
    if( pp_block == NULL || *pp_block == NULL ) return NULL;
    p_block = *pp_block;
    *pp_block = NULL;
    if( p_sys->i_spu_size <= 0 &&
        ( p_block->i_pts <= VLC_TS_INVALID || p_block->i_buffer < 4 ) )
    {
        msg_Dbg( p_dec, "invalid starting packet (size < 4 or pts <=0)" );
        msg_Dbg( p_dec, "spu size: %d, i_pts: %"PRId64" i_buffer: %zu",
                 p_sys->i_spu_size, p_block->i_pts, p_block->i_buffer );
        block_Release( p_block );
        return NULL;
    }
    block_ChainAppend( &p_sys->p_block, p_block );
    p_sys->i_spu += p_block->i_buffer;
    if( p_sys->i_spu_size <= 0 )
    {
        p_sys->i_spu_size = ( p_block->p_buffer[0] << 8 )|
            p_block->p_buffer[1];
        p_sys->i_rle_size = ( ( p_block->p_buffer[2] << 8 )|
            p_block->p_buffer[3] ) - 4;
        /* msg_Dbg( p_dec, "i_spu_size=%d i_rle=%d",
                    p_sys->i_spu_size, p_sys->i_rle_size ); */
        if( p_sys->i_spu_size <= 0 || p_sys->i_rle_size >= p_sys->i_spu_size )
        {
            p_sys->i_spu_size = 0;
            p_sys->i_rle_size = 0;
            p_sys->i_spu      = 0;
            p_sys->p_block    = NULL;
            block_Release( p_block );
            return NULL;
        }
    }
    if( p_sys->i_spu >= p_sys->i_spu_size )
    {
        /* We have a complete sub */
        if( p_sys->i_spu > p_sys->i_spu_size )
            msg_Dbg( p_dec, "SPU packets size=%d should be %d",
                     p_sys->i_spu, p_sys->i_spu_size );
        return p_sys->p_block;
    }
    return NULL;
}
