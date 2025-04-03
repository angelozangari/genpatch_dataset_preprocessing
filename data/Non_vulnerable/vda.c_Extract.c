}
static int Extract( vlc_va_t *external, picture_t *p_picture, void *opaque,
                    uint8_t *data )
{
    vlc_va_vda_t *p_va = vlc_va_vda_Get( external );
    CVPixelBufferRef cv_buffer = ( CVPixelBufferRef )data;
    if( !cv_buffer )
    {
        msg_Dbg( p_va->p_log, "Frame buffer is empty.");
        return VLC_EGENERIC;
    }
    if (!CVPixelBufferGetDataSize(cv_buffer) > 0)
    {
        msg_Dbg( p_va->p_log, "Empty frame buffer");
        return VLC_EGENERIC;
    }
    if( p_va->hw_ctx.cv_pix_fmt_type == kCVPixelFormatType_420YpCbCr8Planar )
    {
        if( !p_va->image_cache.buffer ) {
            CVPixelBufferRelease( cv_buffer );
            return VLC_EGENERIC;
        }
        vda_Copy420YpCbCr8Planar( p_picture,
                                  cv_buffer,
                                  p_va->hw_ctx.width,
                                  p_va->hw_ctx.height,
                                  &p_va->image_cache );
    }
    else
        vda_Copy422YpCbCr8( p_picture, cv_buffer );
    (void) opaque;
    return VLC_SUCCESS;
}
}
static int Extract( vlc_va_t *external, picture_t *p_picture, void *opaque,
                    uint8_t *data )
{
    vlc_va_vda_t *p_va = vlc_va_vda_Get( external );
    CVPixelBufferRef cv_buffer = (CVPixelBufferRef)data;
    if( !cv_buffer )
    {
        msg_Dbg( p_va->p_log, "Frame buffer is empty.");
        return VLC_EGENERIC;
    }
    if (!CVPixelBufferGetDataSize(cv_buffer) > 0)
    {
        msg_Dbg( p_va->p_log, "Empty frame buffer");
        return VLC_EGENERIC;
    }
    vda_Copy422YpCbCr8( p_picture, cv_buffer );
    (void) opaque;
    return VLC_SUCCESS;
}
