 *****************************************************************************/
static vlm_message_t *vlm_ShowMedia( vlm_media_sys_t *p_media )
{
    vlm_media_t *p_cfg = &p_media->cfg;
    vlm_message_t *p_msg;
    vlm_message_t *p_msg_sub;
    int i;
    p_msg = vlm_MessageSimpleNew( p_cfg->psz_name );
    vlm_MessageAdd( p_msg,
                    vlm_MessageNew( "type", p_cfg->b_vod ? "vod" : "broadcast" ) );
    vlm_MessageAdd( p_msg,
                    vlm_MessageNew( "enabled", p_cfg->b_enabled ? "yes" : "no" ) );
    if( p_cfg->b_vod )
        vlm_MessageAdd( p_msg,
                        vlm_MessageNew( "mux", "%s", p_cfg->vod.psz_mux ) );
    else
        vlm_MessageAdd( p_msg,
                        vlm_MessageNew( "loop", p_cfg->broadcast.b_loop ? "yes" : "no" ) );
    p_msg_sub = vlm_MessageAdd( p_msg, vlm_MessageSimpleNew( "inputs" ) );
    for( i = 0; i < p_cfg->i_input; i++ )
    {
        char *psz_tmp;
        if( asprintf( &psz_tmp, "%d", i+1 ) != -1 )
        {
            vlm_MessageAdd( p_msg_sub,
                       vlm_MessageNew( psz_tmp, "%s", p_cfg->ppsz_input[i] ) );
            free( psz_tmp );
        }
    }
    vlm_MessageAdd( p_msg,
                    vlm_MessageNew( "output", "%s", p_cfg->psz_output ? p_cfg->psz_output : "" ) );
    p_msg_sub = vlm_MessageAdd( p_msg, vlm_MessageSimpleNew( "options" ) );
    for( i = 0; i < p_cfg->i_option; i++ )
        vlm_MessageAdd( p_msg_sub, vlm_MessageSimpleNew( p_cfg->ppsz_option[i] ) );
    p_msg_sub = vlm_MessageAdd( p_msg, vlm_MessageSimpleNew( "instances" ) );
    for( i = 0; i < p_media->i_instance; i++ )
    {
        vlm_media_instance_sys_t *p_instance = p_media->instance[i];
        vlc_value_t val;
        vlm_message_t *p_msg_instance;
        val.i_int = END_S;
        if( p_instance->p_input )
            var_Get( p_instance->p_input, "state", &val );
        p_msg_instance = vlm_MessageAdd( p_msg_sub, vlm_MessageSimpleNew( "instance" ) );
        vlm_MessageAdd( p_msg_instance,
                        vlm_MessageNew( "name" , "%s", p_instance->psz_name ? p_instance->psz_name : "default" ) );
        vlm_MessageAdd( p_msg_instance,
                        vlm_MessageNew( "state",
                            val.i_int == PLAYING_S ? "playing" :
                            val.i_int == PAUSE_S ? "paused" :
                            "stopped" ) );
        /* FIXME should not do that this way */
        if( p_instance->p_input )
        {
#define APPEND_INPUT_INFO( key, format, type ) \
            vlm_MessageAdd( p_msg_instance, vlm_MessageNew( key, format, \
                            var_Get ## type( p_instance->p_input, key ) ) )
            APPEND_INPUT_INFO( "position", "%f", Float );
            APPEND_INPUT_INFO( "time", "%"PRIi64, Time );
            APPEND_INPUT_INFO( "length", "%"PRIi64, Time );
            APPEND_INPUT_INFO( "rate", "%f", Float );
            APPEND_INPUT_INFO( "title", "%"PRId64, Integer );
            APPEND_INPUT_INFO( "chapter", "%"PRId64, Integer );
            APPEND_INPUT_INFO( "can-seek", "%d", Bool );
        }
#undef APPEND_INPUT_INFO
        vlm_MessageAdd( p_msg_instance, vlm_MessageNew( "playlistindex",
                        "%d", p_instance->i_index + 1 ) );
    }
    return p_msg;
}
