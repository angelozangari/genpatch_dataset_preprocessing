 *****************************************************************************/
static void *ProcessPacket( decoder_t *p_dec, ogg_packet *p_oggpacket,
                            block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t *p_block = *pp_block;
    /* Date management */
    if( p_block && p_block->i_pts > VLC_TS_INVALID &&
        p_block->i_pts != date_Get( &p_sys->end_date ) )
    {
        date_Set( &p_sys->end_date, p_block->i_pts );
    }
    if( !date_Get( &p_sys->end_date ) )
    {
        /* We've just started the stream, wait for the first PTS. */
        if( p_block ) block_Release( p_block );
        return NULL;
    }
    *pp_block = NULL; /* To avoid being fed the same packet again */
    if( p_sys->b_packetizer )
    {
        return SendPacket( p_dec, p_oggpacket, p_block );
    }
    else
    {
        block_t *p_aout_buffer = DecodePacket( p_dec, p_oggpacket );
        if( p_block )
            block_Release( p_block );
        return p_aout_buffer;
    }
}
