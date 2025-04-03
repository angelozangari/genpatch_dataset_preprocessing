}
static int ExecuteControl( vlm_t *p_vlm, const char *psz_name, const int i_arg, char ** ppsz_arg, vlm_message_t **pp_status )
{
    vlm_media_sys_t *p_media;
    const char *psz_control = NULL;
    const char *psz_instance = NULL;
    const char *psz_argument = NULL;
    int i_index;
    int i_result;
    if( !ExecuteIsMedia( p_vlm, psz_name ) )
    {
        *pp_status = vlm_MessageNew( "control", "%s: media unknown", psz_name );
        return VLC_EGENERIC;
    }
    assert( i_arg > 0 );
#define IS(txt) ( !strcmp( ppsz_arg[i_index], (txt) ) )
    i_index = 0;
    if( !IS("play") && !IS("stop") && !IS("pause") && !IS("seek") )
    {
        i_index = 1;
        psz_instance = ppsz_arg[0];
        if( i_index >= i_arg || ( !IS("play") && !IS("stop") && !IS("pause") && !IS("seek") ) )
            return ExecuteSyntaxError( "control", pp_status );
    }
#undef IS
    psz_control = ppsz_arg[i_index];
    if( i_index+1 < i_arg )
        psz_argument = ppsz_arg[i_index+1];
    p_media = vlm_MediaSearch( p_vlm, psz_name );
    assert( p_media );
    if( !strcmp( psz_control, "play" ) )
    {
        int i_input_index = 0;
        int i;
        if( ( psz_argument && sscanf(psz_argument, "%d", &i) == 1 ) && i > 0 && i-1 < p_media->cfg.i_input )
        {
            i_input_index = i-1;
        }
        else if( psz_argument )
        {
            int j;
            vlm_media_t *p_cfg = &p_media->cfg;
            for ( j=0; j < p_cfg->i_input; j++)
            {
                if( !strcmp( p_cfg->ppsz_input[j], psz_argument ) )
                {
                    i_input_index = j;
                    break;
                }
            }
        }
        if( p_media->cfg.b_vod )
            i_result = vlm_ControlInternal( p_vlm, VLM_START_MEDIA_VOD_INSTANCE, p_media->cfg.id, psz_instance, i_input_index, NULL );    // we should get here now
        else
            i_result = vlm_ControlInternal( p_vlm, VLM_START_MEDIA_BROADCAST_INSTANCE, p_media->cfg.id, psz_instance, i_input_index );
    }
    else if( !strcmp( psz_control, "seek" ) )
    {
        if( psz_argument )
        {
            bool b_relative;
            if( psz_argument[0] == '+' || psz_argument[0] == '-' )
                b_relative = true;
            else
                b_relative = false;
            if( strstr( psz_argument, "ms" ) || strstr( psz_argument, "s" ) )
            {
                /* Time (ms or s) */
                int64_t i_new_time;
                if( strstr( psz_argument, "ms" ) )
                    i_new_time =  1000 * (int64_t)atoi( psz_argument );
                else
                    i_new_time = 1000000 * (int64_t)atoi( psz_argument );
                if( b_relative )
                {
                    int64_t i_time = 0;
                    vlm_ControlInternal( p_vlm, VLM_GET_MEDIA_INSTANCE_TIME, p_media->cfg.id, psz_instance, &i_time );
                    i_new_time += i_time;
                }
                if( i_new_time < 0 )
                    i_new_time = 0;
                i_result = vlm_ControlInternal( p_vlm, VLM_SET_MEDIA_INSTANCE_TIME, p_media->cfg.id, psz_instance, i_new_time );
            }
            else
            {
                /* Percent */
                double d_new_position = us_atof( psz_argument ) / 100.0;
                if( b_relative )
                {
                    double d_position = 0.0;
                    vlm_ControlInternal( p_vlm, VLM_GET_MEDIA_INSTANCE_POSITION, p_media->cfg.id, psz_instance, &d_position );
                    d_new_position += d_position;
                }
                if( d_new_position < 0.0 )
                    d_new_position = 0.0;
                else if( d_new_position > 1.0 )
                    d_new_position = 1.0;
                i_result = vlm_ControlInternal( p_vlm, VLM_SET_MEDIA_INSTANCE_POSITION, p_media->cfg.id, psz_instance, d_new_position );
            }
        }
        else
        {
            i_result = VLC_EGENERIC;
        }
    }
    else if( !strcmp( psz_control, "stop" ) )
    {
        i_result = vlm_ControlInternal( p_vlm, VLM_STOP_MEDIA_INSTANCE, p_media->cfg.id, psz_instance );
    }
    else if( !strcmp( psz_control, "pause" ) )
    {
        i_result = vlm_ControlInternal( p_vlm, VLM_PAUSE_MEDIA_INSTANCE, p_media->cfg.id, psz_instance );
    }
    else
    {
        i_result = VLC_EGENERIC;
    }
    if( i_result )
    {
        *pp_status = vlm_MessageNew( "control", "unknown error" );
        return VLC_SUCCESS;
    }
    *pp_status = vlm_MessageSimpleNew( "control" );
    return VLC_SUCCESS;
}
