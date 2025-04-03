}
static int Create( vlc_va_t *p_va, AVCodecContext *ctx,
                   const es_format_t *fmt )
{
#ifdef VLC_VA_BACKEND_XLIB
    if( !vlc_xlib_init( VLC_OBJECT(p_va) ) )
    {
        msg_Warn( p_va, "Ignoring VA API" );
        return VLC_EGENERIC;
    }
#endif
    (void) fmt;
    int err = Open( p_va, ctx->codec_id, ctx->thread_count );
    if( err )
        return err;
    /* Only VLD supported */
    p_va->pix_fmt = PIX_FMT_VAAPI_VLD;
    p_va->setup = Setup;
    p_va->get = Get;
    p_va->release = Release;
    p_va->extract = Extract;
    return VLC_SUCCESS;
}
