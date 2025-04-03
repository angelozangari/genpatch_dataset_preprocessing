 */
void update_Download( update_t *p_update, const char *psz_destdir )
{
    assert( p_update );
    // If the object already exist, destroy it
    if( p_update->p_download )
    {
        atomic_store( &p_update->p_download->aborted, true );
        vlc_join( p_update->p_download->thread, NULL );
        vlc_object_release( p_update->p_download );
    }
    update_download_thread_t *p_udt =
        vlc_custom_create( p_update->p_libvlc, sizeof( *p_udt ),
                           "update download" );
    if( !p_udt )
        return;
    p_udt->p_update = p_update;
    p_update->p_download = p_udt;
    p_udt->psz_destdir = psz_destdir ? strdup( psz_destdir ) : NULL;
    atomic_store(&p_udt->aborted, false);
    vlc_clone( &p_udt->thread, update_DownloadReal, p_udt, VLC_THREAD_PRIORITY_LOW );
}
}
void update_Download( update_t *p_update, const char *psz_destdir )
{
    (void)p_update; (void)psz_destdir;
}
