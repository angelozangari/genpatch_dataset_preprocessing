 *****************************************************************************/
static block_t *SendPacket( decoder_t *p_dec, ogg_packet *p_oggpacket,
                            block_t *p_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    int i_block_size, i_samples;
    i_block_size = vorbis_packet_blocksize( &p_sys->vi, p_oggpacket );
    if( i_block_size < 0 ) i_block_size = 0; /* non audio packet */
    i_samples = ( p_sys->i_last_block_size + i_block_size ) >> 2;
    p_sys->i_last_block_size = i_block_size;
    /* Date management */
    p_block->i_dts = p_block->i_pts = date_Get( &p_sys->end_date );
    p_block->i_length = date_Increment( &p_sys->end_date, i_samples ) - p_block->i_pts;
    return p_block;
}
