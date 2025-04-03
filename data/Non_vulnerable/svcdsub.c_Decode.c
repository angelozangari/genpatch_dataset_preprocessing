 *****************************************************************************/
static subpicture_t *Decode( decoder_t *p_dec, block_t **pp_block )
{
    block_t *p_block, *p_spu;
    dbg_print( "" );
    if( pp_block == NULL || *pp_block == NULL ) return NULL;
    p_block = *pp_block;
    *pp_block = NULL;
    if( !(p_spu = Reassemble( p_dec, p_block )) ) return NULL;
    /* Parse and decode */
    return DecodePacket( p_dec, p_spu );
}
