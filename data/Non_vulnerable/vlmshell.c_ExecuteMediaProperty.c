}
static int ExecuteMediaProperty( vlm_t *p_vlm, int64_t id, bool b_new,
                                 const int i_property, char *ppsz_property[], vlm_message_t **pp_status )
{
    const char *psz_cmd = b_new ? "new" : "setup";
    vlm_media_t *p_cfg = NULL;
    int i_result;
    int i;
#undef ERROR
#undef MISSING
#define ERROR( txt ) do { *pp_status = vlm_MessageNew( psz_cmd, txt); goto error; } while(0)
    if( vlm_ControlInternal( p_vlm, VLM_GET_MEDIA, id, &p_cfg ) )
        ERROR( "unknown media" );
#define MISSING(cmd) do { if( !psz_value ) ERROR( "missing argument for " cmd ); } while(0)
    for( i = 0; i < i_property; i++ )
    {
        const char *psz_option = ppsz_property[i];
        const char *psz_value = i+1 < i_property ? ppsz_property[i+1] :  NULL;
        if( !strcmp( psz_option, "enabled" ) )
        {
            p_cfg->b_enabled = true;
        }
        else if( !strcmp( psz_option, "disabled" ) )
        {
            p_cfg->b_enabled = false;
        }
        else if( !strcmp( psz_option, "input" ) )
        {
            MISSING( "input" );
            TAB_APPEND( p_cfg->i_input, p_cfg->ppsz_input, strdup(psz_value) );
            i++;
        }
        else if( !strcmp( psz_option, "inputdel" ) && psz_value && !strcmp( psz_value, "all" ) )
        {
            while( p_cfg->i_input > 0 )
                TAB_REMOVE( p_cfg->i_input, p_cfg->ppsz_input, p_cfg->ppsz_input[0] );
            i++;
        }
        else if( !strcmp( psz_option, "inputdel" ) )
        {
            int j;
            MISSING( "inputdel" );
            for( j = 0; j < p_cfg->i_input; j++ )
            {
                if( !strcmp( p_cfg->ppsz_input[j], psz_value ) )
                {
                    TAB_REMOVE( p_cfg->i_input, p_cfg->ppsz_input, p_cfg->ppsz_input[j] );
                    break;
                }
            }
            i++;
        }
        else if( !strcmp( psz_option, "inputdeln" ) )
        {
            MISSING( "inputdeln" );
            int idx = atoi( psz_value );
            if( idx > 0 && idx <= p_cfg->i_input )
                TAB_REMOVE( p_cfg->i_input, p_cfg->ppsz_input, p_cfg->ppsz_input[idx-1] );
            i++;
        }
        else if( !strcmp( psz_option, "output" ) )
        {
            MISSING( "output" );
            free( p_cfg->psz_output );
            p_cfg->psz_output = *psz_value ? strdup( psz_value ) : NULL;
            i++;
        }
        else if( !strcmp( psz_option, "option" ) )
        {
            MISSING( "option" );
            TAB_APPEND( p_cfg->i_option, p_cfg->ppsz_option, strdup( psz_value ) );
            i++;
        }
        else if( !strcmp( psz_option, "loop" ) )
        {
            if( p_cfg->b_vod )
                ERROR( "invalid loop option for vod" );
            p_cfg->broadcast.b_loop = true;
        }
        else if( !strcmp( psz_option, "unloop" ) )
        {
            if( p_cfg->b_vod )
                ERROR( "invalid unloop option for vod" );
            p_cfg->broadcast.b_loop = false;
        }
        else if( !strcmp( psz_option, "mux" ) )
        {
            MISSING( "mux" );
            if( !p_cfg->b_vod )
                ERROR( "invalid mux option for broadcast" );
            free( p_cfg->vod.psz_mux );
            p_cfg->vod.psz_mux = *psz_value ? strdup( psz_value ) : NULL;
            i++;
        }
        else
        {
            fprintf( stderr, "PROP: name=%s unknown\n", psz_option );
            ERROR( "Wrong command syntax" );
        }
    }
#undef MISSING
#undef ERROR
    /* */
    i_result = vlm_ControlInternal( p_vlm, VLM_CHANGE_MEDIA, p_cfg );
    vlm_media_Delete( p_cfg );
    *pp_status = vlm_MessageSimpleNew( psz_cmd );
    return i_result;
error:
    if( p_cfg )
    {
        if( b_new )
            vlm_ControlInternal( p_vlm, VLM_DEL_MEDIA, p_cfg->id );
        vlm_media_Delete( p_cfg );
    }
    return VLC_EGENERIC;
}
