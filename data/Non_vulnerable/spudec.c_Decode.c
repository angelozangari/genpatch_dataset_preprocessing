 *****************************************************************************/
static subpicture_t *Decode( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t       *p_spu_block;
    subpicture_t  *p_spu;
    p_spu_block = Reassemble( p_dec, pp_block );
    if( ! p_spu_block )
    {
        return NULL;
    }
    /* FIXME: what the, we shouldn’t need to allocate 64k of buffer --sam. */
    p_sys->i_spu = block_ChainExtract( p_spu_block, p_sys->buffer, 65536 );
    p_sys->i_pts = p_spu_block->i_pts;
    block_ChainRelease( p_spu_block );
    /* Parse and decode */
    p_spu = ParsePacket( p_dec );
    /* reinit context */
    p_sys->i_spu_size = 0;
    p_sys->i_rle_size = 0;
    p_sys->i_spu      = 0;
    p_sys->p_block    = NULL;
    return p_spu;
}
