}
static void Release( void *opaque, uint8_t *data )
{
    vlc_va_surface_t *p_surface = opaque;
    vlc_mutex_lock( p_surface->p_lock );
    p_surface->i_refcount--;
    vlc_mutex_unlock( p_surface->p_lock );
    (void) data;
}
