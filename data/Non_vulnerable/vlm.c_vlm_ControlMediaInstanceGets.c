}
static int vlm_ControlMediaInstanceGets( vlm_t *p_vlm, int64_t id, vlm_media_instance_t ***ppp_idsc, int *pi_instance )
{
    vlm_media_sys_t *p_media = vlm_ControlMediaGetById( p_vlm, id );
    vlm_media_instance_t **pp_idsc;
    int                              i_idsc;
    int i;
    if( !p_media )
        return VLC_EGENERIC;
    TAB_INIT( i_idsc, pp_idsc );
    for( i = 0; i < p_media->i_instance; i++ )
    {
        vlm_media_instance_sys_t *p_instance = p_media->instance[i];
        vlm_media_instance_t *p_idsc = vlm_media_instance_New();
        if( p_instance->psz_name )
            p_idsc->psz_name = strdup( p_instance->psz_name );
        if( p_instance->p_input )
        {
            p_idsc->i_time = var_GetTime( p_instance->p_input, "time" );
            p_idsc->i_length = var_GetTime( p_instance->p_input, "length" );
            p_idsc->d_position = var_GetFloat( p_instance->p_input, "position" );
            if( var_GetInteger( p_instance->p_input, "state" ) == PAUSE_S )
                p_idsc->b_paused = true;
            p_idsc->i_rate = INPUT_RATE_DEFAULT
                             / var_GetFloat( p_instance->p_input, "rate" );
        }
        TAB_APPEND( i_idsc, pp_idsc, p_idsc );
    }
    *ppp_idsc = pp_idsc;
    *pi_instance = i_idsc;
    return VLC_SUCCESS;
}
