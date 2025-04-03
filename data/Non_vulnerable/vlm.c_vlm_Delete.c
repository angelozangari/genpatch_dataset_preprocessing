 *****************************************************************************/
void vlm_Delete( vlm_t *p_vlm )
{
    /* vlm_Delete() is serialized against itself, and against vlm_New().
     * This mutex protects libvlc_priv->p_vlm and p_vlm->users. */
    vlc_mutex_lock( &vlm_mutex );
    assert( p_vlm->users > 0 );
    if( --p_vlm->users == 0 )
        assert( libvlc_priv(p_vlm->p_libvlc)->p_vlm == p_vlm );
    else
        p_vlm = NULL;
    if( p_vlm == NULL )
    {
        vlc_mutex_unlock( &vlm_mutex );
        return;
    }
    /* Destroy and release VLM */
    vlc_mutex_lock( &p_vlm->lock );
    vlm_ControlInternal( p_vlm, VLM_CLEAR_MEDIAS );
    TAB_CLEAN( p_vlm->i_media, p_vlm->media );
    vlm_ControlInternal( p_vlm, VLM_CLEAR_SCHEDULES );
    TAB_CLEAN( p_vlm->i_schedule, p_vlm->schedule );
    vlc_mutex_unlock( &p_vlm->lock );
    vlc_cancel( p_vlm->thread );
    if( p_vlm->p_vod )
    {
        module_unneed( p_vlm->p_vod, p_vlm->p_vod->p_module );
        vlc_object_release( p_vlm->p_vod );
    }
    libvlc_priv(p_vlm->p_libvlc)->p_vlm = NULL;
    vlc_mutex_unlock( &vlm_mutex );
    vlc_join( p_vlm->thread, NULL );
    vlc_cond_destroy( &p_vlm->wait_manage );
    vlc_mutex_destroy( &p_vlm->lock );
    vlc_mutex_destroy( &p_vlm->lock_manage );
    vlc_object_release( p_vlm );
}
