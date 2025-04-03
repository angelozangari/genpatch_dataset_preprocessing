 *****************************************************************************/
static void Close(vlc_object_t *p_this)
{
    decoder_t *dec = (decoder_t *)p_this;
    decoder_sys_t *sys = dec->p_sys;
    /* Free our PTS */
    const void *iter = NULL;
    for (;;) {
        struct vpx_image *img = vpx_codec_get_frame(&sys->ctx, &iter);
        if (!img)
            break;
        free(img->user_priv);
    }
    vpx_codec_destroy(&sys->ctx);
    free(sys);
}
