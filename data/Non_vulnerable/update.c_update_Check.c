 */
void update_Check( update_t *p_update, void (*pf_callback)( void*, bool ), void *p_data )
{
    assert( p_update );
    // If the object already exist, destroy it
    if( p_update->p_check )
    {
        vlc_join( p_update->p_check->thread, NULL );
        free( p_update->p_check );
    }
    update_check_thread_t *p_uct = calloc( 1, sizeof( *p_uct ) );
    if( !p_uct ) return;
    p_uct->p_update = p_update;
    p_update->p_check = p_uct;
    p_uct->pf_callback = pf_callback;
    p_uct->p_data = p_data;
    vlc_clone( &p_uct->thread, update_CheckReal, p_uct, VLC_THREAD_PRIORITY_LOW );
}
}
void update_Check( update_t *p_update, void (*pf_callback)( void*, bool ),
                   void *p_data )
{
    (void)p_update; (void)pf_callback; (void)p_data;
}
