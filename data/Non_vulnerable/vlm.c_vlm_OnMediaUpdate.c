/* Called after a media description is changed/added */
static int vlm_OnMediaUpdate( vlm_t *p_vlm, vlm_media_sys_t *p_media )
{
    vlm_media_t *p_cfg = &p_media->cfg;
    /* Check if we need to create/delete a vod media */
    if( p_cfg->b_vod && p_vlm->p_vod )
    {
        if( !p_cfg->b_enabled && p_media->vod.p_media )
        {
            p_vlm->p_vod->pf_media_del( p_vlm->p_vod, p_media->vod.p_media );
            p_media->vod.p_media = NULL;
        }
        else if( p_cfg->b_enabled && !p_media->vod.p_media && p_cfg->i_input )
        {
            /* Pre-parse the input */
            input_thread_t *p_input;
            char *psz_output;
            char *psz_header;
            char *psz_dup;
            int i;
            vlc_gc_decref( p_media->vod.p_item );
            if( strstr( p_cfg->ppsz_input[0], "://" ) == NULL )
            {
                char *psz_uri = vlc_path2uri( p_cfg->ppsz_input[0], NULL );
                p_media->vod.p_item = input_item_New( psz_uri,
                                                      p_cfg->psz_name );
                free( psz_uri );
            }
            else
                p_media->vod.p_item = input_item_New( p_cfg->ppsz_input[0],
                                                      p_cfg->psz_name );
            if( p_cfg->psz_output )
            {
                if( asprintf( &psz_output, "%s:description", p_cfg->psz_output )  == -1 )
                    psz_output = NULL;
            }
            else
                psz_output = strdup( "#description" );
            if( psz_output && asprintf( &psz_dup, "sout=%s", psz_output ) != -1 )
            {
                input_item_AddOption( p_media->vod.p_item, psz_dup, VLC_INPUT_OPTION_TRUSTED );
                free( psz_dup );
            }
            free( psz_output );
            for( i = 0; i < p_cfg->i_option; i++ )
                input_item_AddOption( p_media->vod.p_item,
                                      p_cfg->ppsz_option[i], VLC_INPUT_OPTION_TRUSTED );
            if( asprintf( &psz_header, _("Media: %s"), p_cfg->psz_name ) == -1 )
                psz_header = NULL;
            sout_description_data_t data;
            TAB_INIT(data.i_es, data.es);
            p_input = input_Create( p_vlm->p_vod, p_media->vod.p_item, psz_header, NULL );
            if( p_input )
            {
                vlc_sem_t sem_preparse;
                vlc_sem_init( &sem_preparse, 0 );
                preparse_data_t preparse = { .p_sem = &sem_preparse,
                                    .b_mux = (p_cfg->vod.psz_mux != NULL) };
                var_AddCallback( p_input, "intf-event", InputEventPreparse,
                                 &preparse );
                data.sem = &sem_preparse;
                var_Create( p_input, "sout-description-data", VLC_VAR_ADDRESS );
                var_SetAddress( p_input, "sout-description-data", &data );
                if( !input_Start( p_input ) )
                    vlc_sem_wait( &sem_preparse );
                var_DelCallback( p_input, "intf-event", InputEventPreparse,
                                 &preparse );
                input_Stop( p_input, true );
                input_Close( p_input );
                vlc_sem_destroy( &sem_preparse );
            }
            free( psz_header );
            /* XXX: Don't do it that way, but properly use a new input item ref. */
            input_item_t item = *p_media->vod.p_item;;
            if( p_cfg->vod.psz_mux )
            {
                const char *psz_mux;
                if (!strcmp(p_cfg->vod.psz_mux, "ps"))
                    psz_mux = "mp2p";
                else if (!strcmp(p_cfg->vod.psz_mux, "ts"))
                    psz_mux = "mp2t";
                else
                    psz_mux = p_cfg->vod.psz_mux;
                es_format_t es, *p_es = &es;
                union {
                    char text[5];
                    unsigned char utext[5];
                    uint32_t value;
                } fourcc;
                sprintf( fourcc.text, "%4.4s", psz_mux );
                for( int i = 0; i < 4; i++ )
                    fourcc.utext[i] = tolower(fourcc.utext[i]);
                item.i_es = 1;
                item.es = &p_es;
                es_format_Init( &es, VIDEO_ES, fourcc.value );
            }
            else
            {
                item.i_es = data.i_es;
                item.es = data.es;
            }
            p_media->vod.p_media = p_vlm->p_vod->pf_media_new( p_vlm->p_vod,
                                                    p_cfg->psz_name, &item );
            TAB_CLEAN(data.i_es, data.es);
        }
    }
    else if ( p_cfg->b_vod )
        msg_Err( p_vlm, "vod server is not loaded" );
    else
    {
        /* TODO start media if needed */
    }
    /* TODO add support of var vlm_media_broadcast/vlm_media_vod */
    vlm_SendEventMediaChanged( p_vlm, p_cfg->id, p_cfg->psz_name );
    return VLC_SUCCESS;
}
