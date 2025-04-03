}
static int vlm_ControlMediaDel( vlm_t *p_vlm, int64_t id )
{
    vlm_media_sys_t *p_media = vlm_ControlMediaGetById( p_vlm, id );
    if( !p_media )
        return VLC_EGENERIC;
    while( p_media->i_instance > 0 )
        vlm_ControlInternal( p_vlm, VLM_STOP_MEDIA_INSTANCE, id, p_media->instance[0]->psz_name );
    if( p_media->cfg.b_vod )
    {
        p_media->cfg.b_enabled = false;
        vlm_OnMediaUpdate( p_vlm, p_media );
    }
    /* */
    vlm_SendEventMediaRemoved( p_vlm, id, p_media->cfg.psz_name );
    vlm_media_Clean( &p_media->cfg );
    vlc_gc_decref( p_media->vod.p_item );
    if( p_media->vod.p_media )
        p_vlm->p_vod->pf_media_del( p_vlm->p_vod, p_media->vod.p_media );
    TAB_REMOVE( p_vlm->i_media, p_vlm->media, p_media );
    free( p_media );
    return VLC_SUCCESS;
}
