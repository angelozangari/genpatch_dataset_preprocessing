}
static block_t *Encode( encoder_t *p_enc, subpicture_t *p_spu )
{
    VLC_UNUSED( p_enc );
    subpicture_region_t *p_region;
    block_t *p_block;
    size_t len;
    if( p_spu == NULL )
        return NULL;
    p_region = p_spu->p_region;
    if( ( p_region == NULL )
     || ( p_region->fmt.i_chroma != VLC_CODEC_TEXT )
     || ( p_region->psz_text == NULL ) )
        return NULL;
    /* This should already be UTF-8 encoded, so not much effort... */
    len = strlen( p_region->psz_text );
    p_block = block_Alloc( len );
    memcpy( p_block->p_buffer, p_region->psz_text, len );
    p_block->i_pts = p_block->i_dts = p_spu->i_start;
    if( !p_spu->b_ephemer && ( p_spu->i_stop > p_spu->i_start ) )
        p_block->i_length = p_spu->i_stop - p_spu->i_start;
    return p_block;
}
