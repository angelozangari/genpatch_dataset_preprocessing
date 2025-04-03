}
static vlm_media_instance_sys_t *vlm_MediaInstanceNew( vlm_t *p_vlm, const char *psz_name )
{
    vlm_media_instance_sys_t *p_instance = calloc( 1, sizeof(vlm_media_instance_sys_t) );
    if( !p_instance )
        return NULL;
    p_instance->psz_name = NULL;
    if( psz_name )
        p_instance->psz_name = strdup( psz_name );
    p_instance->p_item = input_item_New( NULL, NULL );
    p_instance->i_index = 0;
    p_instance->b_sout_keep = false;
    p_instance->p_parent = vlc_object_create( p_vlm, sizeof (vlc_object_t) );
    p_instance->p_input = NULL;
    p_instance->p_input_resource = input_resource_New( p_instance->p_parent );
    return p_instance;
}
