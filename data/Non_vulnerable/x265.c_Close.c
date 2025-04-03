}
static void Close(vlc_object_t *p_this)
{
    encoder_t     *p_enc = (encoder_t *)p_this;
    encoder_sys_t *p_sys = p_enc->p_sys;
    x265_encoder_close(p_sys->h);
    free(p_sys);
}
