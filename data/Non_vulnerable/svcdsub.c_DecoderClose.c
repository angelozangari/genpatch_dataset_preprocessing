 *****************************************************************************/
void DecoderClose( vlc_object_t *p_this )
{
    decoder_t     *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys = p_dec->p_sys;
    if( p_sys->p_spu ) block_ChainRelease( p_sys->p_spu );
    free( p_sys );
}
