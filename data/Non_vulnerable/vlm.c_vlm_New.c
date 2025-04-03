 *****************************************************************************/
vlm_t *vlm_New ( vlc_object_t *p_this )
{
    vlm_t *p_vlm = NULL, **pp_vlm = &(libvlc_priv (p_this->p_libvlc)->p_vlm);
    char *psz_vlmconf;
    /* Avoid multiple creation */
    vlc_mutex_lock( &vlm_mutex );
    p_vlm = *pp_vlm;
    if( p_vlm )
    {   /* VLM already exists */
        if( likely( p_vlm->users < UINT_MAX ) )
            p_vlm->users++;
        else
            p_vlm = NULL;
        vlc_mutex_unlock( &vlm_mutex );
        return p_vlm;
    }
    msg_Dbg( p_this, "creating VLM" );
    p_vlm = vlc_custom_create( p_this->p_libvlc, sizeof( *p_vlm ),
                               "vlm daemon" );
    if( !p_vlm )
    {
        vlc_mutex_unlock( &vlm_mutex );
        return NULL;
    }
    vlc_mutex_init( &p_vlm->lock );
    vlc_mutex_init( &p_vlm->lock_manage );
    vlc_cond_init_daytime( &p_vlm->wait_manage );
    p_vlm->users = 1;
    p_vlm->input_state_changed = false;
    p_vlm->i_id = 1;
    TAB_INIT( p_vlm->i_media, p_vlm->media );
    TAB_INIT( p_vlm->i_schedule, p_vlm->schedule );
    p_vlm->p_vod = NULL;
    var_Create( p_vlm, "intf-event", VLC_VAR_ADDRESS );
    if( vlc_clone( &p_vlm->thread, Manage, p_vlm, VLC_THREAD_PRIORITY_LOW ) )
    {
        vlc_cond_destroy( &p_vlm->wait_manage );
        vlc_mutex_destroy( &p_vlm->lock );
        vlc_mutex_destroy( &p_vlm->lock_manage );
        vlc_object_release( p_vlm );
        vlc_mutex_unlock( &vlm_mutex );
        return NULL;
    }
    *pp_vlm = p_vlm; /* for future reference */
    /* Load our configuration file */
    psz_vlmconf = var_CreateGetString( p_vlm, "vlm-conf" );
    if( psz_vlmconf && *psz_vlmconf )
    {
        vlm_message_t *p_message = NULL;
        char *psz_buffer = NULL;
        msg_Dbg( p_this, "loading VLM configuration" );
        if( asprintf(&psz_buffer, "load %s", psz_vlmconf ) != -1 )
        {
            msg_Dbg( p_this, "%s", psz_buffer );
            if( vlm_ExecuteCommand( p_vlm, psz_buffer, &p_message ) )
                msg_Warn( p_this, "error while loading the configuration file" );
            vlm_MessageDelete( p_message );
            free( psz_buffer );
        }
    }
    free( psz_vlmconf );
    vlc_mutex_unlock( &vlm_mutex );
    return p_vlm;
}
