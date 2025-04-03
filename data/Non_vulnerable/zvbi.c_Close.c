 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    decoder_t     *p_dec = (decoder_t*) p_this;
    decoder_sys_t *p_sys = p_dec->p_sys;
    var_DelCallback( p_dec, "vbi-position", Position, p_sys );
    var_DelCallback( p_dec, "vbi-opaque", Opaque, p_sys );
    var_DelCallback( p_dec, "vbi-page", RequestPage, p_sys );
    var_DelCallback( p_dec->p_libvlc, "key-pressed", EventKey, p_dec );
    vlc_mutex_destroy( &p_sys->lock );
    if( p_sys->p_vbi_dec )
        vbi_decoder_delete( p_sys->p_vbi_dec );
    free( p_sys );
}
