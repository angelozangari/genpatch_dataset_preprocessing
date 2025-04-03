}
static int CreateSurfaces( vlc_va_sys_t *sys, void **pp_hw_ctx, vlc_fourcc_t *pi_chroma,
                           int i_width, int i_height )
{
    assert( i_width > 0 && i_height > 0 );
    /* */
    sys->p_surface = calloc( sys->i_surface_count, sizeof(*sys->p_surface) );
    if( !sys->p_surface )
        return VLC_EGENERIC;
    sys->image.image_id = VA_INVALID_ID;
    sys->i_context_id   = VA_INVALID_ID;
    /* Create surfaces */
    VASurfaceID pi_surface_id[sys->i_surface_count];
    if( vaCreateSurfaces( sys->p_display, VA_RT_FORMAT_YUV420, i_width, i_height,
                          pi_surface_id, sys->i_surface_count, NULL, 0 ) )
    {
        for( int i = 0; i < sys->i_surface_count; i++ )
            sys->p_surface[i].i_id = VA_INVALID_SURFACE;
        goto error;
    }
    for( int i = 0; i < sys->i_surface_count; i++ )
    {
        vlc_va_surface_t *p_surface = &sys->p_surface[i];
        p_surface->i_id = pi_surface_id[i];
        p_surface->i_refcount = 0;
        p_surface->i_order = 0;
        p_surface->p_lock = &sys->lock;
    }
    /* Create a context */
    if( vaCreateContext( sys->p_display, sys->i_config_id,
                         i_width, i_height, VA_PROGRESSIVE,
                         pi_surface_id, sys->i_surface_count, &sys->i_context_id ) )
    {
        sys->i_context_id = VA_INVALID_ID;
        goto error;
    }
    /* Find and create a supported image chroma */
    int i_fmt_count = vaMaxNumImageFormats( sys->p_display );
    VAImageFormat *p_fmt = calloc( i_fmt_count, sizeof(*p_fmt) );
    if( !p_fmt )
        goto error;
    if( vaQueryImageFormats( sys->p_display, p_fmt, &i_fmt_count ) )
    {
        free( p_fmt );
        goto error;
    }
    VAImage test_image;
    if(vaDeriveImage(sys->p_display, pi_surface_id[0], &test_image) == VA_STATUS_SUCCESS)
    {
        sys->b_supports_derive = true;
        vaDestroyImage(sys->p_display, test_image.image_id);
    }
    vlc_fourcc_t  i_chroma = 0;
    for( int i = 0; i < i_fmt_count; i++ )
    {
        if( p_fmt[i].fourcc == VA_FOURCC_YV12 ||
            p_fmt[i].fourcc == VA_FOURCC_IYUV ||
            p_fmt[i].fourcc == VA_FOURCC_NV12 )
        {
            if( vaCreateImage(  sys->p_display, &p_fmt[i], i_width, i_height, &sys->image ) )
            {
                sys->image.image_id = VA_INVALID_ID;
                continue;
            }
            /* Validate that vaGetImage works with this format */
            if( vaGetImage( sys->p_display, pi_surface_id[0],
                            0, 0, i_width, i_height,
                            sys->image.image_id) )
            {
                vaDestroyImage( sys->p_display, sys->image.image_id );
                sys->image.image_id = VA_INVALID_ID;
                continue;
            }
            i_chroma = VLC_CODEC_YV12;
            break;
        }
    }
    free( p_fmt );
    if( !i_chroma )
        goto error;
    *pi_chroma = i_chroma;
    if(sys->b_supports_derive)
    {
        vaDestroyImage( sys->p_display, sys->image.image_id );
        sys->image.image_id = VA_INVALID_ID;
    }
    if( unlikely(CopyInitCache( &sys->image_cache, i_width )) )
        goto error;
    /* Setup the ffmpeg hardware context */
    *pp_hw_ctx = &sys->hw_ctx;
    memset( &sys->hw_ctx, 0, sizeof(sys->hw_ctx) );
    sys->hw_ctx.display    = sys->p_display;
    sys->hw_ctx.config_id  = sys->i_config_id;
    sys->hw_ctx.context_id = sys->i_context_id;
    /* */
    sys->i_surface_chroma = i_chroma;
    sys->i_surface_width = i_width;
    sys->i_surface_height = i_height;
    return VLC_SUCCESS;
error:
    DestroySurfaces( sys );
    return VLC_EGENERIC;
}
