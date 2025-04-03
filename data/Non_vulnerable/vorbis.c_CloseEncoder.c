 *****************************************************************************/
static void CloseEncoder( vlc_object_t *p_this )
{
    encoder_t *p_enc = (encoder_t *)p_this;
    encoder_sys_t *p_sys = p_enc->p_sys;
    vorbis_block_clear( &p_sys->vb );
    vorbis_dsp_clear( &p_sys->vd );
    vorbis_comment_clear( &p_sys->vc );
    vorbis_info_clear( &p_sys->vi );  /* must be called last */
    free( p_sys );
}
