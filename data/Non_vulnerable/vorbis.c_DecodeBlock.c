 ****************************************************************************/
static block_t *DecodeBlock( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    ogg_packet oggpacket;
    if( !pp_block ) return NULL;
    if( *pp_block )
    {
        /* Block to Ogg packet */
        oggpacket.packet = (*pp_block)->p_buffer;
        oggpacket.bytes = (*pp_block)->i_buffer;
    }
    else
    {
        if( p_sys->b_packetizer ) return NULL;
        /* Block to Ogg packet */
        oggpacket.packet = NULL;
        oggpacket.bytes = 0;
    }
    oggpacket.granulepos = -1;
    oggpacket.b_o_s = 0;
    oggpacket.e_o_s = 0;
    oggpacket.packetno = 0;
    /* Check for headers */
    if( !p_sys->b_has_headers )
    {
        if( ProcessHeaders( p_dec ) )
        {
            if( *pp_block )
                block_Release( *pp_block );
            return NULL;
        }
        p_sys->b_has_headers = true;
    }
    return ProcessPacket( p_dec, &oggpacket, pp_block );
}
