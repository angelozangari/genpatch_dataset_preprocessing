 *****************************************************************************/
static void CloseDecoder( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t *)p_this;
    decoder_sys_t *p_sys = p_dec->p_sys;
    th_info_clear(&p_sys->ti);
    th_comment_clear(&p_sys->tc);
    th_decode_free(p_sys->tcx);
    p_sys->tcx = NULL;
    free( p_sys );
}
