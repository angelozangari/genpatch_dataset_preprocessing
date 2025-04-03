 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    filter_t     *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys;
    char *psz_effects, *psz_parser;
    p_sys = p_filter->p_sys = malloc( sizeof( filter_sys_t ) );
    if( unlikely (p_sys == NULL ) )
        return VLC_EGENERIC;
    int width = var_InheritInteger( p_filter , "effect-width");
    int height = var_InheritInteger( p_filter , "effect-width");
    /* No resolution under 400x532 and no odd dimension */
    if( width < 532 )
        width  = 532;
    width &= ~1;
    if( height < 400 )
        height = 400;
    height &= ~1;
    p_sys->i_effect = 0;
    p_sys->effect   = NULL;
    /* Parse the effect list */
    psz_parser = psz_effects = var_CreateGetString( p_filter, "effect-list" );
    while( psz_parser && *psz_parser != '\0' )
    {
        visual_effect_t *p_effect;
        p_effect = malloc( sizeof( visual_effect_t ) );
        if( !p_effect )
            break;
        p_effect->i_width     = width;
        p_effect->i_height    = height;
        p_effect->i_nb_chans  = aout_FormatNbChannels( &p_filter->fmt_in.audio);
        p_effect->i_idx_left  = 0;
        p_effect->i_idx_right = __MIN( 1, p_effect->i_nb_chans-1 );
        p_effect->p_data   = NULL;
        p_effect->pf_run   = NULL;
        for( unsigned i = 0; i < effectc; i++ )
        {
            if( !strncasecmp( psz_parser, effectv[i].name,
                              strlen( effectv[i].name ) ) )
            {
                p_effect->pf_run = effectv[i].run_cb;
                p_effect->pf_free = effectv[i].free_cb;
                psz_parser += strlen( effectv[i].name );
                break;
            }
        }
        if( p_effect->pf_run != NULL )
        {
            if( *psz_parser == '{' )
            {
                char *psz_eoa;
                psz_parser++;
                if( ( psz_eoa = strchr( psz_parser, '}') ) == NULL )
                {
                   msg_Err( p_filter, "unable to parse effect list. Aborting");
                   free( p_effect );
                   break;
                }
            }
            TAB_APPEND( p_sys->i_effect, p_sys->effect, p_effect );
        }
        else
        {
            msg_Err( p_filter, "unknown visual effect: %s", psz_parser );
            free( p_effect );
        }
        if( strchr( psz_parser, ',' ) )
        {
            psz_parser = strchr( psz_parser, ',' ) + 1;
        }
        else if( strchr( psz_parser, ':' ) )
        {
            psz_parser = strchr( psz_parser, ':' ) + 1;
        }
        else
        {
            break;
        }
    }
    free( psz_effects );
    if( !p_sys->i_effect )
    {
        msg_Err( p_filter, "no effects found" );
        goto error;
    }
    /* Open the video output */
    video_format_t fmt = {
        .i_chroma = VLC_CODEC_I420,
        .i_width = width,
        .i_height = height,
        .i_visible_width = width,
        .i_visible_height = height,
        .i_sar_num = 1,
        .i_sar_den = 1,
    };
    p_sys->p_vout = aout_filter_RequestVout( p_filter, NULL, &fmt );
    if( p_sys->p_vout == NULL )
    {
        msg_Err( p_filter, "no suitable vout module" );
        goto error;
    }
    p_sys->fifo = block_FifoNew();
    if( unlikely( p_sys->fifo == NULL ) )
    {
        aout_filter_RequestVout( p_filter, p_sys->p_vout, NULL );
        goto error;
    }
    if( vlc_clone( &p_sys->thread, Thread, p_filter,
                   VLC_THREAD_PRIORITY_VIDEO ) )
    {
        block_FifoRelease( p_sys->fifo );
        aout_filter_RequestVout( p_filter, p_sys->p_vout, NULL );
        goto error;
    }
    p_filter->fmt_in.audio.i_format = VLC_CODEC_FL32;
    p_filter->fmt_out.audio = p_filter->fmt_in.audio;
    p_filter->pf_audio_filter = DoWork;
    return VLC_SUCCESS;
error:
    for( int i = 0; i < p_sys->i_effect; i++ )
        free( p_sys->effect[i] );
    free( p_sys->effect );
    free( p_sys );
    return VLC_EGENERIC;
}
