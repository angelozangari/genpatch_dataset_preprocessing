 *****************************************************************************/
static void *ProcessPacket( decoder_t *p_dec, ogg_packet *p_oggpacket,
                            block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t *p_block = *pp_block;
    void *p_buf;
    if( ( p_block->i_flags&(BLOCK_FLAG_DISCONTINUITY|BLOCK_FLAG_CORRUPTED) ) != 0 )
    {
        /* Don't send the the first packet after a discontinuity to
         * theora_decode, otherwise we get purple/green display artifacts
         * appearing in the video output */
        block_Release(p_block);
        return NULL;
    }
    /* Date management */
    if( p_block->i_pts > VLC_TS_INVALID && p_block->i_pts != p_sys->i_pts )
    {
        p_sys->i_pts = p_block->i_pts;
    }
    *pp_block = NULL; /* To avoid being fed the same packet again */
    if( p_sys->b_packetizer )
    {
        /* Date management */
        p_block->i_dts = p_block->i_pts = p_sys->i_pts;
        p_block->i_length = p_sys->i_pts - p_block->i_pts;
        p_buf = p_block;
    }
    else
    {
        p_buf = DecodePacket( p_dec, p_oggpacket );
        if( p_block )
            block_Release( p_block );
    }
    /* Date management */
    p_sys->i_pts += ( INT64_C(1000000) * p_sys->ti.fps_denominator /
                      p_sys->ti.fps_numerator ); /* 1 frame per packet */
    return p_buf;
}
