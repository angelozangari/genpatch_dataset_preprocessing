}
static int vlm_ControlMediaInstancePause( vlm_t *p_vlm, int64_t id, const char *psz_id )
{
    vlm_media_sys_t *p_media = vlm_ControlMediaGetById( p_vlm, id );
    vlm_media_instance_sys_t *p_instance;
    int i_state;
    if( !p_media )
        return VLC_EGENERIC;
    p_instance = vlm_ControlMediaInstanceGetByName( p_media, psz_id );
    if( !p_instance || !p_instance->p_input )
        return VLC_EGENERIC;
    /* Toggle pause state */
    i_state = var_GetInteger( p_instance->p_input, "state" );
    if( i_state == PAUSE_S && !p_media->cfg.b_vod )
        var_SetInteger( p_instance->p_input, "state", PLAYING_S );
    else if( i_state == PLAYING_S )
        var_SetInteger( p_instance->p_input, "state", PAUSE_S );
    return VLC_SUCCESS;
}
