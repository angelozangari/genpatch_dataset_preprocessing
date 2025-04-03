}
static int vlm_ControlMediaAdd( vlm_t *p_vlm, vlm_media_t *p_cfg, int64_t *p_id )
{
    vlm_media_sys_t *p_media;
    if( vlm_MediaDescriptionCheck( p_vlm, p_cfg ) || vlm_ControlMediaGetByName( p_vlm, p_cfg->psz_name ) )
    {
        msg_Err( p_vlm, "invalid media description" );
        return VLC_EGENERIC;
    }
    /* Check if we need to load the VOD server */
    if( p_cfg->b_vod && !p_vlm->p_vod )
    {
        p_vlm->p_vod = vlc_custom_create( VLC_OBJECT(p_vlm), sizeof( vod_t ),
                                          "vod server" );
        p_vlm->p_vod->p_module = module_need( p_vlm->p_vod, "vod server", "$vod-server", false );
        if( !p_vlm->p_vod->p_module )
        {
            msg_Err( p_vlm, "cannot find vod server" );
            vlc_object_release( p_vlm->p_vod );
            p_vlm->p_vod = NULL;
            return VLC_EGENERIC;
        }
        p_vlm->p_vod->p_data = p_vlm;
        p_vlm->p_vod->pf_media_control = vlm_MediaVodControl;
    }
    p_media = calloc( 1, sizeof( vlm_media_sys_t ) );
    if( !p_media )
        return VLC_ENOMEM;
    vlm_media_Copy( &p_media->cfg, p_cfg );
    p_media->cfg.id = p_vlm->i_id++;
    /* FIXME do we do something here if enabled is true ? */
    p_media->vod.p_item = input_item_New( NULL, NULL );
    p_media->vod.p_media = NULL;
    TAB_INIT( p_media->i_instance, p_media->instance );
    /* */
    TAB_APPEND( p_vlm->i_media, p_vlm->media, p_media );
    if( p_id )
        *p_id = p_media->cfg.id;
    /* */
    vlm_SendEventMediaAdded( p_vlm, p_media->cfg.id, p_media->cfg.psz_name );
    return vlm_OnMediaUpdate( p_vlm, p_media );
}
