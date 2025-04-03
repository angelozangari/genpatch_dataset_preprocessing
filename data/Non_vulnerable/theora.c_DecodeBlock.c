 ****************************************************************************/
static void *DecodeBlock( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t *p_block;
    ogg_packet oggpacket;
    if( !pp_block || !*pp_block ) return NULL;
    p_block = *pp_block;
    /* Block to Ogg packet */
    oggpacket.packet = p_block->p_buffer;
    oggpacket.bytes = p_block->i_buffer;
    oggpacket.granulepos = p_block->i_dts;
    oggpacket.b_o_s = 0;
    oggpacket.e_o_s = 0;
    oggpacket.packetno = 0;
    /* Check for headers */
    if( !p_sys->b_has_headers )
    {
        if( ProcessHeaders( p_dec ) )
        {
            block_Release( *pp_block );
            return NULL;
        }
        p_sys->b_has_headers = true;
    }
    return ProcessPacket( p_dec, &oggpacket, pp_block );
}
