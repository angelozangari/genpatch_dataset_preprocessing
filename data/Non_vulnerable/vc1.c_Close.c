 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    decoder_t     *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys = p_dec->p_sys;
    packetizer_Clean( &p_sys->packetizer );
    if( p_sys->p_frame )
        block_Release( p_sys->p_frame );
    free( p_sys );
}
