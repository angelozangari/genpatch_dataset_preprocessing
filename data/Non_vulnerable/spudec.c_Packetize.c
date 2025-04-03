 *****************************************************************************/
static block_t *Packetize( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t       *p_spu = Reassemble( p_dec, pp_block );
    if( ! p_spu )
    {
        return NULL;
    }
    p_spu->i_dts = p_spu->i_pts;
    p_spu->i_length = 0;
    /* reinit context */
    p_sys->i_spu_size = 0;
    p_sys->i_rle_size = 0;
    p_sys->i_spu      = 0;
    p_sys->p_block    = NULL;
    return block_ChainGather( p_spu );
}
