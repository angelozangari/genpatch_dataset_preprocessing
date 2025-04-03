}
static int vlm_ControlMediaInstanceStart( vlm_t *p_vlm, int64_t id, const char *psz_id, int i_input_index, const char *psz_vod_output )
{
    vlm_media_sys_t *p_media = vlm_ControlMediaGetById( p_vlm, id );
    vlm_media_instance_sys_t *p_instance;
    char *psz_log;
    if( !p_media || !p_media->cfg.b_enabled || p_media->cfg.i_input <= 0 )
        return VLC_EGENERIC;
    /* TODO support multiple input for VOD with sout-keep ? */
    if( ( p_media->cfg.b_vod && !psz_vod_output ) || ( !p_media->cfg.b_vod && psz_vod_output ) )
        return VLC_EGENERIC;
    if( i_input_index < 0 || i_input_index >= p_media->cfg.i_input )
        return VLC_EGENERIC;
    p_instance = vlm_ControlMediaInstanceGetByName( p_media, psz_id );
    if( !p_instance )
    {
        vlm_media_t *p_cfg = &p_media->cfg;
        int i;
        p_instance = vlm_MediaInstanceNew( p_vlm, psz_id );
        if( !p_instance )
            return VLC_ENOMEM;
        if ( p_cfg->b_vod )
        {
            var_Create( p_instance->p_parent, "vod-media", VLC_VAR_ADDRESS );
            var_SetAddress( p_instance->p_parent, "vod-media",
                            p_media->vod.p_media );
            var_Create( p_instance->p_parent, "vod-session", VLC_VAR_STRING );
            var_SetString( p_instance->p_parent, "vod-session", psz_id );
        }
        if( p_cfg->psz_output != NULL || psz_vod_output != NULL )
        {
            char *psz_buffer;
            if( asprintf( &psz_buffer, "sout=%s%s%s",
                      p_cfg->psz_output ? p_cfg->psz_output : "",
                      (p_cfg->psz_output && psz_vod_output) ? ":" : psz_vod_output ? "#" : "",
                      psz_vod_output ? psz_vod_output : "" ) != -1 )
            {
                input_item_AddOption( p_instance->p_item, psz_buffer, VLC_INPUT_OPTION_TRUSTED );
                free( psz_buffer );
            }
        }
        for( i = 0; i < p_cfg->i_option; i++ )
        {
            if( !strcmp( p_cfg->ppsz_option[i], "sout-keep" ) )
                p_instance->b_sout_keep = true;
            else if( !strcmp( p_cfg->ppsz_option[i], "nosout-keep" ) || !strcmp( p_cfg->ppsz_option[i], "no-sout-keep" ) )
                p_instance->b_sout_keep = false;
            else
                input_item_AddOption( p_instance->p_item, p_cfg->ppsz_option[i], VLC_INPUT_OPTION_TRUSTED );
        }
        TAB_APPEND( p_media->i_instance, p_media->instance, p_instance );
    }
    /* Stop old instance */
    input_thread_t *p_input = p_instance->p_input;
    if( p_input )
    {
        if( p_instance->i_index == i_input_index &&
            !p_input->b_eof && !p_input->b_error )
        {
            if( var_GetInteger( p_input, "state" ) == PAUSE_S )
                var_SetInteger( p_input, "state",  PLAYING_S );
            return VLC_SUCCESS;
        }
        input_Stop( p_input, true );
        input_Join( p_input );
        var_DelCallback( p_instance->p_input, "intf-event", InputEvent, p_media );
        input_Release( p_input );
        if( !p_instance->b_sout_keep )
            input_resource_TerminateSout( p_instance->p_input_resource );
        input_resource_TerminateVout( p_instance->p_input_resource );
        vlm_SendEventMediaInstanceStopped( p_vlm, id, p_media->cfg.psz_name );
    }
    /* Start new one */
    p_instance->i_index = i_input_index;
    if( strstr( p_media->cfg.ppsz_input[p_instance->i_index], "://" ) == NULL )
    {
        char *psz_uri = vlc_path2uri(
                          p_media->cfg.ppsz_input[p_instance->i_index], NULL );
        input_item_SetURI( p_instance->p_item, psz_uri ) ;
        free( psz_uri );
    }
    else
        input_item_SetURI( p_instance->p_item, p_media->cfg.ppsz_input[p_instance->i_index] ) ;
    if( asprintf( &psz_log, _("Media: %s"), p_media->cfg.psz_name ) != -1 )
    {
        p_instance->p_input = input_Create( p_instance->p_parent,
                                            p_instance->p_item, psz_log,
                                            p_instance->p_input_resource );
        if( p_instance->p_input )
        {
            var_AddCallback( p_instance->p_input, "intf-event", InputEvent, p_media );
            if( input_Start( p_instance->p_input ) != VLC_SUCCESS )
            {
                var_DelCallback( p_instance->p_input, "intf-event", InputEvent, p_media );
                vlc_object_release( p_instance->p_input );
                p_instance->p_input = NULL;
            }
        }
        if( !p_instance->p_input )
        {
            vlm_MediaInstanceDelete( p_vlm, id, p_instance, p_media );
        }
        else
        {
            vlm_SendEventMediaInstanceStarted( p_vlm, id, p_media->cfg.psz_name );
        }
        free( psz_log );
    }
    return VLC_SUCCESS;
}
