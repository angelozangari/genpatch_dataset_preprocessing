 *****************************************************************************/
static block_t *Packetize( decoder_t *p_dec, block_t **pp_block )
{
    block_t *p_block, *p_spu;
    if( pp_block == NULL || *pp_block == NULL ) return NULL;
    p_block = *pp_block;
    *pp_block = NULL;
    if( !(p_spu = Reassemble( p_dec, p_block )) ) return NULL;
    p_spu->i_dts = p_spu->i_pts;
    p_spu->i_length = 0;
    return p_spu;
}
