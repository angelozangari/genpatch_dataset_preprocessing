}
static void Close( vlc_va_sys_t *sys )
{
    if( sys->i_surface_width || sys->i_surface_height )
        DestroySurfaces( sys );
    if( sys->i_config_id != VA_INVALID_ID )
        vaDestroyConfig( sys->p_display, sys->i_config_id );
    vaTerminate( sys->p_display );
#ifdef VLC_VA_BACKEND_XLIB
    XCloseDisplay( sys->p_display_x11 );
#endif
#ifdef VLC_VA_BACKEND_DRM
    close( sys->drm_fd );
#endif
}
