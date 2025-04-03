}
static int Setup( vlc_va_t *external, void **pp_hw_ctx, vlc_fourcc_t *pi_chroma,
                 int i_width, int i_height )
{
    vlc_va_vda_t *p_va = vlc_va_vda_Get( external );
    if( p_va->hw_ctx.width == i_width
       && p_va->hw_ctx.height == i_height
       && p_va->hw_ctx.decoder )
    {
        *pp_hw_ctx = &p_va->hw_ctx;
        *pi_chroma = p_va->i_chroma;
        return VLC_SUCCESS;
    }
    if( p_va->hw_ctx.decoder )
    {
        ff_vda_destroy_decoder( &p_va->hw_ctx );
        goto ok;
    }
    memset( &p_va->hw_ctx, 0, sizeof(p_va->hw_ctx) );
    p_va->hw_ctx.format = 'avc1';
    p_va->hw_ctx.use_ref_buffer = 1;
    int i_pix_fmt = var_CreateGetInteger( p_va->p_log, "avcodec-vda-pix-fmt" );
    switch( i_pix_fmt )
    {
        case 1 :
            p_va->hw_ctx.cv_pix_fmt_type = kCVPixelFormatType_422YpCbCr8;
            p_va->i_chroma = VLC_CODEC_UYVY;
            msg_Dbg(p_va->p_log, "using pixel format 422YpCbCr8");
            break;
        case 0 :
        default :
            p_va->hw_ctx.cv_pix_fmt_type = kCVPixelFormatType_420YpCbCr8Planar;
            p_va->i_chroma = VLC_CODEC_I420;
            CopyInitCache( &p_va->image_cache, i_width );
            msg_Dbg(p_va->p_log, "using pixel format 420YpCbCr8Planar");
    }
ok:
    /* Setup the libavcodec hardware context */
    *pp_hw_ctx = &p_va->hw_ctx;
    *pi_chroma = p_va->i_chroma;
    p_va->hw_ctx.width = i_width;
    p_va->hw_ctx.height = i_height;
    /* create the decoder */
    int status = ff_vda_create_decoder( &p_va->hw_ctx,
                                       p_va->p_extradata,
                                       p_va->i_extradata );
    if( status )
    {
        msg_Err( p_va->p_log, "Failed to create decoder: %i", status );
        return VLC_EGENERIC;
    }
    else
        msg_Dbg( p_va->p_log, "VDA decoder created");
    return VLC_SUCCESS;
}
}
static int Setup( vlc_va_t *external, void **pp_hw_ctx, vlc_fourcc_t *pi_chroma,
                 int i_width, int i_height )
{
    VLC_UNUSED( pp_hw_ctx );
    vlc_va_vda_t *p_va = vlc_va_vda_Get( external );
    *pi_chroma = VLC_CODEC_UYVY;
    av_vda_default_free(p_va->avctx);
    if( av_vda_default_init(p_va->avctx) < 0 )
        return VLC_EGENERIC;
    (void)i_width; (void)i_height;
    return VLC_SUCCESS;
}
