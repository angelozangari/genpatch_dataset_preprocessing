}
static void DestroySurfaces( vlc_va_sys_t *sys )
{
    if( sys->image.image_id != VA_INVALID_ID )
    {
        CopyCleanCache( &sys->image_cache );
        vaDestroyImage( sys->p_display, sys->image.image_id );
    }
    else if(sys->b_supports_derive)
    {
        CopyCleanCache( &sys->image_cache );
    }
    if( sys->i_context_id != VA_INVALID_ID )
        vaDestroyContext( sys->p_display, sys->i_context_id );
    for( int i = 0; i < sys->i_surface_count && sys->p_surface; i++ )
    {
        vlc_va_surface_t *p_surface = &sys->p_surface[i];
        if( p_surface->i_id != VA_INVALID_SURFACE )
            vaDestroySurfaces( sys->p_display, &p_surface->i_id, 1 );
    }
    free( sys->p_surface );
    /* */
    sys->image.image_id = VA_INVALID_ID;
    sys->i_context_id = VA_INVALID_ID;
    sys->p_surface = NULL;
    sys->i_surface_width = 0;
    sys->i_surface_height = 0;
    vlc_mutex_destroy(&sys->lock);
}
