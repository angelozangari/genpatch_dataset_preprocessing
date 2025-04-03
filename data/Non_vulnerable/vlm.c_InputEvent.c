}
static int InputEvent( vlc_object_t *p_this, char const *psz_cmd,
                       vlc_value_t oldval, vlc_value_t newval,
                       void *p_data )
{
    VLC_UNUSED(psz_cmd);
    VLC_UNUSED(oldval);
    input_thread_t *p_input = (input_thread_t *)p_this;
    vlm_t *p_vlm = libvlc_priv( p_input->p_libvlc )->p_vlm;
    assert( p_vlm );
    vlm_media_sys_t *p_media = p_data;
    const char *psz_instance_name = NULL;
    if( newval.i_int == INPUT_EVENT_STATE )
    {
        for( int i = 0; i < p_media->i_instance; i++ )
        {
            if( p_media->instance[i]->p_input == p_input )
            {
                psz_instance_name = p_media->instance[i]->psz_name;
                break;
            }
        }
        vlm_SendEventMediaInstanceState( p_vlm, p_media->cfg.id, p_media->cfg.psz_name, psz_instance_name, var_GetInteger( p_input, "state" ) );
        vlc_mutex_lock( &p_vlm->lock_manage );
        p_vlm->input_state_changed = true;
        vlc_cond_signal( &p_vlm->wait_manage );
        vlc_mutex_unlock( &p_vlm->lock_manage );
    }
    return VLC_SUCCESS;
}
