}
static int vlm_ControlMediaInstanceGetTimePosition( vlm_t *p_vlm, int64_t id, const char *psz_id, int64_t *pi_time, double *pd_position )
{
    vlm_media_sys_t *p_media = vlm_ControlMediaGetById( p_vlm, id );
    vlm_media_instance_sys_t *p_instance;
    if( !p_media )
        return VLC_EGENERIC;
    p_instance = vlm_ControlMediaInstanceGetByName( p_media, psz_id );
    if( !p_instance || !p_instance->p_input )
        return VLC_EGENERIC;
    if( pi_time )
        *pi_time = var_GetTime( p_instance->p_input, "time" );
    if( pd_position )
        *pd_position = var_GetFloat( p_instance->p_input, "position" );
    return VLC_SUCCESS;
}
