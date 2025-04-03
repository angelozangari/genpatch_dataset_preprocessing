}
static int vlm_ControlMediaChange( vlm_t *p_vlm, vlm_media_t *p_cfg )
{
    vlm_media_sys_t *p_media = vlm_ControlMediaGetById( p_vlm, p_cfg->id );
    /* */
    if( !p_media || vlm_MediaDescriptionCheck( p_vlm, p_cfg ) )
        return VLC_EGENERIC;
    if( ( p_media->cfg.b_vod && !p_cfg->b_vod ) || ( !p_media->cfg.b_vod && p_cfg->b_vod ) )
        return VLC_EGENERIC;
    if( 0 )
    {
        /* TODO check what are the changes being done (stop instance if needed) */
    }
    vlm_media_Clean( &p_media->cfg );
    vlm_media_Copy( &p_media->cfg, p_cfg );
    return vlm_OnMediaUpdate( p_vlm, p_media );
}
