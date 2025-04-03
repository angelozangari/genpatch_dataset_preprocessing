 *****************************************************************************/
static void CloseEncoder( vlc_object_t *p_this )
{
    encoder_t *p_enc = (encoder_t *)p_this;
    encoder_sys_t *p_sys = p_enc->p_sys;
    th_info_clear(&p_sys->ti);
    th_comment_clear(&p_sys->tc);
    th_encode_free(p_sys->tcx);
    p_sys->tcx = NULL;
    free( p_sys );
}
