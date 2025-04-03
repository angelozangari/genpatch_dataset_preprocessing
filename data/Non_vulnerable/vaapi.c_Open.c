/* */
static int Open( vlc_va_t *va, int i_codec_id, int i_thread_count )
{
    vlc_va_sys_t *sys = calloc( 1, sizeof(*sys) );
    if ( unlikely(sys == NULL) )
       return VLC_ENOMEM;
    VAProfile i_profile, *p_profiles_list;
    bool b_supported_profile = false;
    int i_profiles_nb = 0;
    int i_surface_count;
    /* */
    switch( i_codec_id )
    {
    case AV_CODEC_ID_MPEG1VIDEO:
    case AV_CODEC_ID_MPEG2VIDEO:
        i_profile = VAProfileMPEG2Main;
        i_surface_count = 2 + 2;
        break;
    case AV_CODEC_ID_MPEG4:
        i_profile = VAProfileMPEG4AdvancedSimple;
        i_surface_count = 2+1;
        break;
    case AV_CODEC_ID_WMV3:
        i_profile = VAProfileVC1Main;
        i_surface_count = 2+1;
        break;
    case AV_CODEC_ID_VC1:
        i_profile = VAProfileVC1Advanced;
        i_surface_count = 2+1;
        break;
    case AV_CODEC_ID_H264:
        i_profile = VAProfileH264High;
        i_surface_count = 16 + i_thread_count + 2;
        break;;
    default:
        free( sys );
        return VLC_EGENERIC;
    }
    /* */
    sys->i_config_id  = VA_INVALID_ID;
    sys->i_context_id = VA_INVALID_ID;
    sys->image.image_id = VA_INVALID_ID;
    /* Create a VA display */
#ifdef VLC_VA_BACKEND_XLIB
    sys->p_display_x11 = XOpenDisplay(NULL);
    if( !sys->p_display_x11 )
    {
        msg_Err( va, "Could not connect to X server" );
        goto error;
    }
    sys->p_display = vaGetDisplay( sys->p_display_x11 );
#endif
#ifdef VLC_VA_BACKEND_DRM
    sys->drm_fd = vlc_open("/dev/dri/card0", O_RDWR);
    if( sys->drm_fd == -1 )
    {
        msg_Err( va, "Could not access rendering device: %m" );
        goto error;
    }
    sys->p_display = vaGetDisplayDRM( sys->drm_fd );
#endif
    if( !sys->p_display )
    {
        msg_Err( va, "Could not get a VAAPI device" );
        goto error;
    }
    int major, minor;
    if( vaInitialize( sys->p_display, &major, &minor ) )
    {
        msg_Err( va, "Failed to initialize the VAAPI device" );
        goto error;
    }
    /* Check if the selected profile is supported */
    i_profiles_nb = vaMaxNumProfiles( sys->p_display );
    p_profiles_list = calloc( i_profiles_nb, sizeof( VAProfile ) );
    if( !p_profiles_list )
        goto error;
    VAStatus i_status = vaQueryConfigProfiles( sys->p_display, p_profiles_list, &i_profiles_nb );
    if ( i_status == VA_STATUS_SUCCESS )
    {
        for( int i = 0; i < i_profiles_nb; i++ )
        {
            if ( p_profiles_list[i] == i_profile )
            {
                b_supported_profile = true;
                break;
            }
        }
    }
    free( p_profiles_list );
    if ( !b_supported_profile )
    {
        msg_Dbg( va, "Codec and profile not supported by the hardware" );
        goto error;
    }
    /* Create a VA configuration */
    VAConfigAttrib attrib;
    memset( &attrib, 0, sizeof(attrib) );
    attrib.type = VAConfigAttribRTFormat;
    if( vaGetConfigAttributes( sys->p_display,
                               i_profile, VAEntrypointVLD, &attrib, 1 ) )
        goto error;
    /* Not sure what to do if not, I don't have a way to test */
    if( (attrib.value & VA_RT_FORMAT_YUV420) == 0 )
        goto error;
    if( vaCreateConfig( sys->p_display,
                        i_profile, VAEntrypointVLD, &attrib, 1, &sys->i_config_id ) )
    {
        sys->i_config_id = VA_INVALID_ID;
        goto error;
    }
    sys->i_surface_count = i_surface_count;
    sys->b_supports_derive = false;
    vlc_mutex_init(&sys->lock);
    va->sys = sys;
    va->description = vaQueryVendorString( sys->p_display );
    return VLC_SUCCESS;
error:
    if( sys->p_display != NULL )
        vaTerminate( sys->p_display );
#ifdef VLC_VA_BACKEND_XLIB
    if( sys->p_display_x11 != NULL )
        XCloseDisplay( sys->p_display_x11 );
#endif
#ifdef VLC_VA_BACKEND_DRM
    if( sys->drm_fd != -1 )
        close( sys->drm_fd );
#endif
    free( sys );
    return VLC_EGENERIC;
}
