 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    sout_stream_t     *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys;
    char              *psz_string;
    if( !p_stream->p_next )
    {
        msg_Err( p_stream, "cannot create chain" );
        return VLC_EGENERIC;
    }
    p_sys = calloc( 1, sizeof( *p_sys ) );
    p_sys->i_master_drift = 0;
    config_ChainParse( p_stream, SOUT_CFG_PREFIX, ppsz_sout_options,
                   p_stream->p_cfg );
    /* Audio transcoding parameters */
    psz_string = var_GetString( p_stream, SOUT_CFG_PREFIX "aenc" );
    p_sys->psz_aenc = NULL;
    p_sys->p_audio_cfg = NULL;
    if( psz_string && *psz_string )
    {
        char *psz_next;
        psz_next = config_ChainCreate( &p_sys->psz_aenc, &p_sys->p_audio_cfg,
                                       psz_string );
        free( psz_next );
    }
    free( psz_string );
    psz_string = var_GetString( p_stream, SOUT_CFG_PREFIX "acodec" );
    p_sys->i_acodec = 0;
    if( psz_string && *psz_string )
    {
        char fcc[5] = "    \0";
        memcpy( fcc, psz_string, __MIN( strlen( psz_string ), 4 ) );
        p_sys->i_acodec = vlc_fourcc_GetCodecFromString( AUDIO_ES, fcc );
        msg_Dbg( p_stream, "Checking codec mapping for %s got %4.4s ", fcc, (char*)&p_sys->i_acodec);
    }
    free( psz_string );
    p_sys->psz_alang = var_GetNonEmptyString( p_stream, SOUT_CFG_PREFIX "alang" );
    p_sys->i_abitrate = var_GetInteger( p_stream, SOUT_CFG_PREFIX "ab" );
    if( p_sys->i_abitrate < 4000 ) p_sys->i_abitrate *= 1000;
    p_sys->i_sample_rate = var_GetInteger( p_stream, SOUT_CFG_PREFIX "samplerate" );
    p_sys->i_channels = var_GetInteger( p_stream, SOUT_CFG_PREFIX "channels" );
    if( p_sys->i_acodec )
    {
        if( ( p_sys->i_acodec == VLC_CODEC_MP3 ||
              p_sys->i_acodec == VLC_CODEC_MP2 ||
              p_sys->i_acodec == VLC_CODEC_MPGA ) && p_sys->i_channels > 2 )
        {
            msg_Warn( p_stream, "%d channels invalid for mp2/mp3, forcing to 2",
                      p_sys->i_channels );
            p_sys->i_channels = 2;
        }
        msg_Dbg( p_stream, "codec audio=%4.4s %dHz %d channels %dKb/s",
                 (char *)&p_sys->i_acodec, p_sys->i_sample_rate,
                 p_sys->i_channels, p_sys->i_abitrate / 1000 );
    }
    psz_string = var_GetString( p_stream, SOUT_CFG_PREFIX "afilter" );
    if( psz_string && *psz_string )
        p_sys->psz_af = strdup( psz_string );
    else
        p_sys->psz_af = NULL;
    free( psz_string );
    /* Video transcoding parameters */
    psz_string = var_GetString( p_stream, SOUT_CFG_PREFIX "venc" );
    p_sys->psz_venc = NULL;
    p_sys->p_video_cfg = NULL;
    if( psz_string && *psz_string )
    {
        char *psz_next;
        psz_next = config_ChainCreate( &p_sys->psz_venc, &p_sys->p_video_cfg,
                                   psz_string );
        free( psz_next );
    }
    free( psz_string );
    psz_string = var_GetString( p_stream, SOUT_CFG_PREFIX "vcodec" );
    p_sys->i_vcodec = 0;
    if( psz_string && *psz_string )
    {
        char fcc[5] = "    \0";
        memcpy( fcc, psz_string, __MIN( strlen( psz_string ), 4 ) );
        p_sys->i_vcodec = vlc_fourcc_GetCodecFromString( VIDEO_ES, fcc );
        msg_Dbg( p_stream, "Checking video codec mapping for %s got %4.4s ", fcc, (char*)&p_sys->i_vcodec);
    }
    free( psz_string );
    p_sys->i_vbitrate = var_GetInteger( p_stream, SOUT_CFG_PREFIX "vb" );
    if( p_sys->i_vbitrate < 16000 ) p_sys->i_vbitrate *= 1000;
    p_sys->f_scale = var_GetFloat( p_stream, SOUT_CFG_PREFIX "scale" );
    p_sys->b_master_sync = var_InheritURational( p_stream, &p_sys->fps_num, &p_sys->fps_den, SOUT_CFG_PREFIX "fps" ) == VLC_SUCCESS;
    p_sys->i_width = var_GetInteger( p_stream, SOUT_CFG_PREFIX "width" );
    p_sys->i_height = var_GetInteger( p_stream, SOUT_CFG_PREFIX "height" );
    p_sys->i_maxwidth = var_GetInteger( p_stream, SOUT_CFG_PREFIX "maxwidth" );
    p_sys->i_maxheight = var_GetInteger( p_stream, SOUT_CFG_PREFIX "maxheight" );
    psz_string = var_GetString( p_stream, SOUT_CFG_PREFIX "vfilter" );
    if( psz_string && *psz_string )
        p_sys->psz_vf2 = strdup(psz_string );
    else
        p_sys->psz_vf2 = NULL;
    free( psz_string );
    p_sys->b_deinterlace = var_GetBool( p_stream, SOUT_CFG_PREFIX "deinterlace" );
    psz_string = var_GetString( p_stream, SOUT_CFG_PREFIX "deinterlace-module" );
    p_sys->psz_deinterlace = NULL;
    p_sys->p_deinterlace_cfg = NULL;
    if( psz_string && *psz_string )
    {
        char *psz_next;
        psz_next = config_ChainCreate( &p_sys->psz_deinterlace,
                                   &p_sys->p_deinterlace_cfg,
                                   psz_string );
        free( psz_next );
    }
    free( psz_string );
    p_sys->i_threads = var_GetInteger( p_stream, SOUT_CFG_PREFIX "threads" );
    p_sys->b_high_priority = var_GetBool( p_stream, SOUT_CFG_PREFIX "high-priority" );
    if( p_sys->i_vcodec )
    {
        msg_Dbg( p_stream, "codec video=%4.4s %dx%d scaling: %f %dkb/s",
                 (char *)&p_sys->i_vcodec, p_sys->i_width, p_sys->i_height,
                 p_sys->f_scale, p_sys->i_vbitrate / 1000 );
    }
    /* Disable hardware decoding by default (unlike normal playback) */
    psz_string = var_CreateGetString( p_stream, "avcodec-hw" );
    if( !strcasecmp( "any", psz_string ) )
        var_SetString( p_stream, "avcodec-hw", "none" );
    free( psz_string );
    /* Subpictures transcoding parameters */
    p_sys->p_spu = NULL;
    p_sys->p_spu_blend = NULL;
    p_sys->psz_senc = NULL;
    p_sys->p_spu_cfg = NULL;
    p_sys->i_scodec = 0;
    psz_string = var_GetString( p_stream, SOUT_CFG_PREFIX "senc" );
    if( psz_string && *psz_string )
    {
        char *psz_next;
        psz_next = config_ChainCreate( &p_sys->psz_senc, &p_sys->p_spu_cfg,
                                   psz_string );
        free( psz_next );
    }
    free( psz_string );
    psz_string = var_GetString( p_stream, SOUT_CFG_PREFIX "scodec" );
    if( psz_string && *psz_string )
    {
        char fcc[5] = "    \0";
        memcpy( fcc, psz_string, __MIN( strlen( psz_string ), 4 ) );
        p_sys->i_scodec = vlc_fourcc_GetCodecFromString( SPU_ES, fcc );
        msg_Dbg( p_stream, "Checking spu codec mapping for %s got %4.4s ", fcc, (char*)&p_sys->i_scodec);
    }
    free( psz_string );
    if( p_sys->i_scodec )
    {
        msg_Dbg( p_stream, "codec spu=%4.4s", (char *)&p_sys->i_scodec );
    }
    p_sys->b_soverlay = var_GetBool( p_stream, SOUT_CFG_PREFIX "soverlay" );
    psz_string = var_GetString( p_stream, SOUT_CFG_PREFIX "sfilter" );
    if( psz_string && *psz_string )
    {
        p_sys->p_spu = spu_Create( p_stream );
        if( p_sys->p_spu )
            spu_ChangeSources( p_sys->p_spu, psz_string );
    }
    free( psz_string );
    /* OSD menu transcoding parameters */
    p_sys->psz_osdenc = NULL;
    p_sys->p_osd_cfg  = NULL;
    p_sys->i_osdcodec = 0;
    p_sys->b_osd   = var_GetBool( p_stream, SOUT_CFG_PREFIX "osd" );
    if( p_sys->b_osd )
    {
        char *psz_next;
        psz_next = config_ChainCreate( &p_sys->psz_osdenc,
                                   &p_sys->p_osd_cfg, "dvbsub" );
        free( psz_next );
        p_sys->i_osdcodec = VLC_CODEC_YUVP;
        msg_Dbg( p_stream, "codec osd=%4.4s", (char *)&p_sys->i_osdcodec );
        if( !p_sys->p_spu )
        {
            p_sys->p_spu = spu_Create( p_stream );
            if( p_sys->p_spu )
                spu_ChangeSources( p_sys->p_spu, "osdmenu" );
        }
        else
        {
            spu_ChangeSources( p_sys->p_spu, "osdmenu" );
        }
    }
    p_stream->pf_add    = Add;
    p_stream->pf_del    = Del;
    p_stream->pf_send   = Send;
    p_stream->p_sys     = p_sys;
    return VLC_SUCCESS;
}
