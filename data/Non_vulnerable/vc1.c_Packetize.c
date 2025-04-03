 *****************************************************************************/
static block_t *Packetize( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    if( p_sys->b_check_startcode && pp_block && *pp_block )
    {
        /* Fix syntax for (some?) VC1 from asf */
        const unsigned i_startcode = sizeof(p_vc1_startcode);
        block_t *p_block = *pp_block;
        if( p_block->i_buffer > 0 &&
            ( p_block->i_buffer < i_startcode ||
              memcmp( p_block->p_buffer, p_vc1_startcode, i_startcode ) ) )
        {
            *pp_block = p_block = block_Realloc( p_block, i_startcode+1, p_block->i_buffer );
            if( p_block )
            {
                memcpy( p_block->p_buffer, p_vc1_startcode, i_startcode );
                if( p_sys->b_sequence_header && p_sys->sh.b_interlaced &&
                    p_block->i_buffer > i_startcode+1 &&
                    (p_block->p_buffer[i_startcode+1] & 0xc0) == 0xc0 )
                    p_block->p_buffer[i_startcode] = IDU_TYPE_FIELD;
                else
                    p_block->p_buffer[i_startcode] = IDU_TYPE_FRAME;
            }
        }
        p_sys->b_check_startcode = false;
    }
    block_t *p_au = packetizer_Packetize( &p_sys->packetizer, pp_block );
    if( !p_au )
        p_sys->b_check_startcode = p_dec->fmt_in.b_packetized;
    return p_au;
}
