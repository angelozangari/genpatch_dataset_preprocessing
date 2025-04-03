 *****************************************************************************/
static void CloseDecoder( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t *)p_this;
    decoder_sys_t *p_sys = p_dec->p_sys;
    if( !p_sys->b_packetizer && p_sys->b_has_headers )
    {
        vorbis_block_clear( &p_sys->vb );
        vorbis_dsp_clear( &p_sys->vd );
    }
    vorbis_comment_clear( &p_sys->vc );
    vorbis_info_clear( &p_sys->vi );  /* must be called last */
    free( p_sys );
}
