}
static int Extract( vlc_va_t *va, picture_t *p_picture, void *opaque,
                    uint8_t *data )
{
    vlc_va_sys_t *sys = va->sys;
    VASurfaceID i_surface_id = (VASurfaceID)(uintptr_t)data;
#if VA_CHECK_VERSION(0,31,0)
    if( vaSyncSurface( sys->p_display, i_surface_id ) )
#else
    if( vaSyncSurface( sys->p_display, sys->i_context_id, i_surface_id ) )
#endif
        return VLC_EGENERIC;
    if(sys->b_supports_derive)
    {
        if(vaDeriveImage(sys->p_display, i_surface_id, &(sys->image)) != VA_STATUS_SUCCESS)
            return VLC_EGENERIC;
    }
    else
    {
        if( vaGetImage( sys->p_display, i_surface_id,
                        0, 0, sys->i_surface_width, sys->i_surface_height,
                        sys->image.image_id) )
            return VLC_EGENERIC;
    }
    void *p_base;
    if( vaMapBuffer( sys->p_display, sys->image.buf, &p_base ) )
        return VLC_EGENERIC;
    const uint32_t i_fourcc = sys->image.format.fourcc;
    if( i_fourcc == VA_FOURCC_YV12 ||
        i_fourcc == VA_FOURCC_IYUV )
    {
        bool b_swap_uv = i_fourcc == VA_FOURCC_IYUV;
        uint8_t *pp_plane[3];
        size_t  pi_pitch[3];
        for( int i = 0; i < 3; i++ )
        {
            const int i_src_plane = (b_swap_uv && i != 0) ?  (3 - i) : i;
            pp_plane[i] = (uint8_t*)p_base + sys->image.offsets[i_src_plane];
            pi_pitch[i] = sys->image.pitches[i_src_plane];
        }
        CopyFromYv12( p_picture, pp_plane, pi_pitch,
                      sys->i_surface_width,
                      sys->i_surface_height,
                      &sys->image_cache );
    }
    else
    {
        assert( i_fourcc == VA_FOURCC_NV12 );
        uint8_t *pp_plane[2];
        size_t  pi_pitch[2];
        for( int i = 0; i < 2; i++ )
        {
            pp_plane[i] = (uint8_t*)p_base + sys->image.offsets[i];
            pi_pitch[i] = sys->image.pitches[i];
        }
        CopyFromNv12( p_picture, pp_plane, pi_pitch,
                      sys->i_surface_width,
                      sys->i_surface_height,
                      &sys->image_cache );
    }
    if( vaUnmapBuffer( sys->p_display, sys->image.buf ) )
        return VLC_EGENERIC;
    if(sys->b_supports_derive)
    {
        vaDestroyImage( sys->p_display, sys->image.image_id );
        sys->image.image_id = VA_INVALID_ID;
    }
    (void) opaque;
    return VLC_SUCCESS;
}
