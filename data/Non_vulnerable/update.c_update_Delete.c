 */
void update_Delete( update_t *p_update )
{
    assert( p_update );
    if( p_update->p_check )
    {
        vlc_join( p_update->p_check->thread, NULL );
        free( p_update->p_check );
    }
    if( p_update->p_download )
    {
        atomic_store( &p_update->p_download->aborted, true );
        vlc_join( p_update->p_download->thread, NULL );
        vlc_object_release( p_update->p_download );
    }
    vlc_mutex_destroy( &p_update->lock );
    free( p_update->release.psz_url );
    free( p_update->release.psz_desc );
    free( p_update->p_pkey );
    free( p_update );
}
}
void update_Delete( update_t *p_update )
{
    (void)p_update;
}
