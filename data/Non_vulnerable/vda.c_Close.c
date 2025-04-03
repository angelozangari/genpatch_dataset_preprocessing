}
static void Close( vlc_va_t *external )
{
    vlc_va_vda_t *p_va = vlc_va_vda_Get( external );
    msg_Dbg(p_va->p_log, "destroying VDA decoder");
    ff_vda_destroy_decoder( &p_va->hw_ctx ) ;
    if( p_va->hw_ctx.cv_pix_fmt_type == kCVPixelFormatType_420YpCbCr8Planar )
        CopyCleanCache( &p_va->image_cache );
    free( p_va );
}
}
static void Close( vlc_va_t *external )
{
    vlc_va_vda_t *p_va = vlc_va_vda_Get( external );
    av_vda_default_free(p_va->avctx);
}
