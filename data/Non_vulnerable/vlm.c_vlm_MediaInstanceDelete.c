}
static void vlm_MediaInstanceDelete( vlm_t *p_vlm, int64_t id, vlm_media_instance_sys_t *p_instance, vlm_media_sys_t *p_media )
{
    input_thread_t *p_input = p_instance->p_input;
    if( p_input )
    {
        input_Stop( p_input, true );
        input_Join( p_input );
        var_DelCallback( p_instance->p_input, "intf-event", InputEvent, p_media );
        input_Release( p_input );
        vlm_SendEventMediaInstanceStopped( p_vlm, id, p_media->cfg.psz_name );
    }
    input_resource_Terminate( p_instance->p_input_resource );
    input_resource_Release( p_instance->p_input_resource );
    vlc_object_release( p_instance->p_parent );
    TAB_REMOVE( p_media->i_instance, p_media->instance, p_instance );
    vlc_gc_decref( p_instance->p_item );
    free( p_instance->psz_name );
    free( p_instance );
}
