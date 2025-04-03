 */
update_t *update_New( vlc_object_t *p_this )
{
    update_t *p_update;
    assert( p_this );
    p_update = (update_t *)malloc( sizeof( update_t ) );
    if( !p_update ) return NULL;
    vlc_mutex_init( &p_update->lock );
    p_update->p_libvlc = p_this->p_libvlc;
    p_update->release.psz_url = NULL;
    p_update->release.psz_desc = NULL;
    p_update->p_download = NULL;
    p_update->p_check = NULL;
    p_update->p_pkey = NULL;
    vlc_gcrypt_init();
    return p_update;
}
#undef update_New
update_t *update_New( vlc_object_t *p_this )
{
    (void)p_this;
    return NULL;
}
