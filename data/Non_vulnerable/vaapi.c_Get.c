}
static int Get( vlc_va_t *va, void **opaque, uint8_t **data )
{
    vlc_va_sys_t *sys = va->sys;
    int i_old;
    int i;
    vlc_mutex_lock( &sys->lock );
    /* Grab an unused surface, in case none are, try the oldest
     * XXX using the oldest is a workaround in case a problem happens with ffmpeg */
    for( i = 0, i_old = 0; i < sys->i_surface_count; i++ )
    {
        vlc_va_surface_t *p_surface = &sys->p_surface[i];
        if( !p_surface->i_refcount )
            break;
        if( p_surface->i_order < sys->p_surface[i_old].i_order )
            i_old = i;
    }
    if( i >= sys->i_surface_count )
        i = i_old;
    vlc_mutex_unlock( &sys->lock );
    vlc_va_surface_t *p_surface = &sys->p_surface[i];
    p_surface->i_refcount = 1;
    p_surface->i_order = sys->i_surface_order++;
    *data = (void *)(uintptr_t)p_surface->i_id;
    *opaque = p_surface;
    return VLC_SUCCESS;
}
